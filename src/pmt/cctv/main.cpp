#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "argh/argh.h"
#include "pmt/cctv/camera_capture.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

namespace pmt::cctv {

namespace {

static volatile std::sig_atomic_t g_running = 1;
static uint64_t g_first_pts_ns = 0;

struct CaptureConfig {
 std::string _device_path = "/dev/video2";
 std::string _dest_dir = "recordings";
 std::chrono::seconds _segment_duration{3600};
 int _retention_days = 30;
 int _overlay_interval_sec = 2;
 std::string _codec_name = "libx265";
 std::string _codec_preset = "medium";
 std::string _codec_crf = "23";
 std::string _overlay_text_path = "/tmp/cctv_timestamp.txt";
};

void ff_check(int err, const char* where) {
 if (err < 0) {
  char buf[256];
  av_strerror(err, buf, sizeof(buf));
  std::cerr << where << " failed: " << buf << "\n";
  std::exit(EXIT_FAILURE);
 }
}

auto make_pts_ms(uint64_t now_ns) -> int64_t {
 if (g_first_pts_ns == 0) {
  g_first_pts_ns = now_ns;
 }

 uint64_t rel_ns = now_ns - g_first_pts_ns;
 uint64_t ms = rel_ns / 1'000'000ULL;

 return static_cast<int64_t>(ms);
}

void reset_pts_origin() {
 g_first_pts_ns = 0;
}

uint64_t quantize_ns(uint64_t ns, uint64_t interval_sec) {
 static uint64_t first_ns = 0;
 if (first_ns == 0)
  first_ns = ns;
 uint64_t rel_ns = ns - first_ns;
 uint64_t rel_s = rel_ns / 1'000'000'000ULL;
 rel_s -= rel_s % interval_sec;
 return first_ns + rel_s * 1'000'000'000ULL;
}

std::string format_timestamp(uint64_t ns_since_epoch) {
 auto const ns = std::chrono::nanoseconds(ns_since_epoch);
 auto const tp = std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(ns));
 auto const time = std::chrono::system_clock::to_time_t(tp);
 auto const local_time = *std::localtime(&time);

 std::ostringstream oss;
 oss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
 return oss.str();
}

std::filesystem::path make_segment_path(std::chrono::system_clock::time_point start_time, const std::string& dest_root) {
 auto tt = std::chrono::system_clock::to_time_t(start_time);
 auto tm = *std::localtime(&tt);

 char day_buf[16];
 std::strftime(day_buf, sizeof(day_buf), "%y_%m_%d", &tm);

 char time_buf[16];
 std::strftime(time_buf, sizeof(time_buf), "%H_%M_%S", &tm);

 std::filesystem::path day_dir = std::filesystem::path(dest_root) / day_buf;
 std::filesystem::create_directories(day_dir);

 return day_dir / (std::string(time_buf) + ".mkv");
}

void prune_recordings(const std::filesystem::path& root, int retention_days) {
 if (retention_days <= 0) {
  return;
 }

 auto collect_day_dirs = [&]() {
  std::vector<std::filesystem::path> dirs;
  if (!std::filesystem::exists(root)) {
   return dirs;
  }
  for (auto const& entry : std::filesystem::directory_iterator(root)) {
   if (entry.is_directory()) {
    dirs.push_back(entry.path());
   }
  }
  std::sort(dirs.begin(), dirs.end());
  return dirs;
 };

 auto remove_empty_dirs = [&]() {
  if (!std::filesystem::exists(root)) {
   return;
  }
  for (auto it = std::filesystem::directory_iterator(root); it != std::filesystem::directory_iterator(); ++it) {
   if (it->is_directory() && std::filesystem::is_empty(it->path())) {
    std::filesystem::remove(it->path());
   }
  }
 };

 while (true) {
  auto day_dirs = collect_day_dirs();
  if (day_dirs.size() <= static_cast<size_t>(retention_days)) {
   break;
  }

  std::filesystem::path oldest_day = day_dirs.front();
  std::vector<std::filesystem::directory_entry> files;
  for (auto const& entry : std::filesystem::directory_iterator(oldest_day)) {
   if (entry.is_regular_file()) {
    files.push_back(entry);
   }
  }
  std::sort(files.begin(), files.end(), [](auto const& a, auto const& b) { return a.path() < b.path(); });

  if (!files.empty()) {
   std::filesystem::remove(files.front().path());
  }
  if (std::filesystem::is_empty(oldest_day)) {
   std::filesystem::remove(oldest_day);
  }
 }

 remove_empty_dirs();
}

void write_textfile_if_changed(const std::string& path, const std::string& text) {
 static std::string last;
 if (text == last)
  return;
 last = text;

 std::ofstream out(path, std::ios::trunc);
 if (!out) {
  std::cerr << "Failed to write textfile: " << path << "\n";
  return;
 }
 out << text;
}

int init_filters(AVFilterGraph** graph, AVFilterContext** buffersrc_ctx, AVFilterContext** buffersink_ctx, AVCodecContext* enc_ctx, int width, int height, AVRational time_base, const char* textfile_path) {
 *graph = nullptr;
 *buffersrc_ctx = nullptr;
 *buffersink_ctx = nullptr;

 int ret = 0;

 char args[256];
 std::snprintf(args, sizeof(args), "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=1/1", width, height, enc_ctx->pix_fmt, time_base.num, time_base.den);

 AVFilterGraph* fg = avfilter_graph_alloc();
 if (!fg)
  return AVERROR(ENOMEM);

 const AVFilter* buffersrc = avfilter_get_by_name("buffer");
 const AVFilter* buffersink = avfilter_get_by_name("buffersink");
 if (!buffersrc || !buffersink) {
  avfilter_graph_free(&fg);
  return AVERROR_FILTER_NOT_FOUND;
 }

 AVFilterContext* src_ctx = nullptr;
 AVFilterContext* sink_ctx = nullptr;

 ret = avfilter_graph_create_filter(&src_ctx, buffersrc, "in", args, nullptr, fg);
 if (ret < 0) {
  avfilter_graph_free(&fg);
  return ret;
 }

 ret = avfilter_graph_create_filter(&sink_ctx, buffersink, "out", nullptr, nullptr, fg);
 if (ret < 0) {
  avfilter_graph_free(&fg);
  return ret;
 }

 char filter_desc[512];
 std::snprintf(filter_desc, sizeof(filter_desc),
               "drawtext="
               "fontfile=/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf:"
               "textfile='%s':"
               "reload=1:"
               "x=10:y=10:fontsize=24:"
               "fontcolor=white@0.9:box=1:boxcolor=black@0.5:boxborderw=4",
               textfile_path);

 AVFilterInOut* outputs = avfilter_inout_alloc();
 AVFilterInOut* inputs = avfilter_inout_alloc();
 if (!outputs || !inputs) {
  avfilter_inout_free(&outputs);
  avfilter_inout_free(&inputs);
  avfilter_graph_free(&fg);
  return AVERROR(ENOMEM);
 }

 outputs->name = av_strdup("in");
 outputs->filter_ctx = src_ctx;
 outputs->pad_idx = 0;
 outputs->next = nullptr;

 inputs->name = av_strdup("out");
 inputs->filter_ctx = sink_ctx;
 inputs->pad_idx = 0;
 inputs->next = nullptr;

 ret = avfilter_graph_parse_ptr(fg, filter_desc, &inputs, &outputs, nullptr);
 avfilter_inout_free(&inputs);
 avfilter_inout_free(&outputs);

 if (ret < 0) {
  avfilter_graph_free(&fg);
  return ret;
 }

 ret = avfilter_graph_config(fg, nullptr);
 if (ret < 0) {
  avfilter_graph_free(&fg);
  return ret;
 }

 *graph = fg;
 *buffersrc_ctx = src_ctx;
 *buffersink_ctx = sink_ctx;
 return 0;
}

void handle_sigint(int) {
 g_running = 0;
}

int record_segment(const CaptureConfig& config, CameraCapture& camera, CameraCapture::Resolution const& res, const std::filesystem::path& output_path) {
 std::string const overlay_text_path = config._overlay_text_path;
 reset_pts_origin();

 AVFormatContext* fmt_ctx = nullptr;
 std::string output_path_str = output_path.string();
 ff_check(avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, output_path_str.c_str()), "avformat_alloc_output_context2");
 if (!fmt_ctx) {
  std::cerr << "Failed to alloc output context\n";
  return 1;
 }

 AVStream* st = avformat_new_stream(fmt_ctx, nullptr);
 if (!st) {
  std::cerr << "Failed to create stream\n";
  return 1;
 }

 st->id = fmt_ctx->nb_streams - 1;
 st->time_base = AVRational{1, 1000};

 const AVCodec* codec = avcodec_find_encoder_by_name(config._codec_name.c_str());
 if (!codec) {
  std::cerr << config._codec_name << " encoder not found\n";
  return 1;
 }

 AVCodecContext* enc_ctx = avcodec_alloc_context3(codec);
 if (!enc_ctx) {
  std::cerr << "Failed to alloc codec context\n";
  return 1;
 }

 enc_ctx->codec_id = codec->id;
 enc_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
 enc_ctx->width = res._width;
 enc_ctx->height = res._height;
 enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
 enc_ctx->time_base = st->time_base;

 av_opt_set(enc_ctx->priv_data, "preset", config._codec_preset.c_str(), 0);
 av_opt_set(enc_ctx->priv_data, "crf", config._codec_crf.c_str(), 0);

 if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
  enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

 std::string open_msg = "avcodec_open2(" + config._codec_name + ")";
 ff_check(avcodec_open2(enc_ctx, codec, nullptr), open_msg.c_str());
 ff_check(avcodec_parameters_from_context(st->codecpar, enc_ctx), "avcodec_parameters_from_context");

 if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
  ff_check(avio_open(&fmt_ctx->pb, output_path_str.c_str(), AVIO_FLAG_WRITE), "avio_open");
 }

 ff_check(avformat_write_header(fmt_ctx, nullptr), "avformat_write_header");

 SwsContext* sws = nullptr;
 AVPixelFormat sws_src_fmt = AV_PIX_FMT_NONE;
 int sws_src_w = 0;
 int sws_src_h = 0;

 AVFrame* yuv_frame = av_frame_alloc();
 if (!yuv_frame) {
  std::cerr << "Failed to alloc AVFrame\n";
  return 1;
 }
 yuv_frame->format = enc_ctx->pix_fmt;
 yuv_frame->width = res._width;
 yuv_frame->height = res._height;
 ff_check(av_frame_get_buffer(yuv_frame, 32), "av_frame_get_buffer");

 AVFilterGraph* filter_graph = nullptr;
 AVFilterContext* buffersrc_ctx = nullptr;
 AVFilterContext* buffersink_ctx = nullptr;

 bool filters_inited = false;
 int const overlay_interval_sec = config._overlay_interval_sec;

 using ClockType = std::chrono::steady_clock;
 auto start_time = ClockType::now();
 size_t frame_count = 0;

 auto process_ready_frame = [&](uint64_t frame_time_ns, ClockType::time_point now_tp) {
  if (!filters_inited) {
   ff_check(init_filters(&filter_graph, &buffersrc_ctx, &buffersink_ctx, enc_ctx, res._width, res._height, st->time_base, overlay_text_path.c_str()), "init_filters");
   filters_inited = true;
  }

  yuv_frame->pts = make_pts_ms(frame_time_ns);

  ff_check(av_buffersrc_add_frame_flags(buffersrc_ctx, yuv_frame, AV_BUFFERSRC_FLAG_KEEP_REF), "av_buffersrc_add_frame");

  while (true) {
   AVFrame* filt_frame = av_frame_alloc();
   if (!filt_frame) {
    std::cerr << "Failed to alloc filt_frame\n";
    std::exit(EXIT_FAILURE);
   }

   int ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
   if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
    av_frame_free(&filt_frame);
    break;
   }
   ff_check(ret, "av_buffersink_get_frame");

   ff_check(avcodec_send_frame(enc_ctx, filt_frame), "avcodec_send_frame(filtered)");
   av_frame_free(&filt_frame);

   while (true) {
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    int ret2 = avcodec_receive_packet(enc_ctx, &pkt);
    if (ret2 == AVERROR(EAGAIN) || ret2 == AVERROR_EOF) {
     av_packet_unref(&pkt);
     break;
    }
    ff_check(ret2, "avcodec_receive_packet");

    av_packet_rescale_ts(&pkt, enc_ctx->time_base, st->time_base);
    pkt.stream_index = st->index;

    ff_check(av_interleaved_write_frame(fmt_ctx, &pkt), "av_interleaved_write_frame");
    av_packet_unref(&pkt);
   }

   frame_count++;
   if (frame_count % 100 == 0) {
    double const seconds = std::chrono::duration<double>(now_tp - start_time).count();
    double const fps = seconds > 0.0 ? frame_count / seconds : 0.0;
    std::cout << "\rFrames: " << frame_count << "  approx FPS: " << fps << std::flush;
   }
  }
 };

 while (g_running) {
  auto frame_lease = camera.get_frame();
  CameraCapture::Frame const& frame = frame_lease.frame();

  ClockType::time_point now = frame._steady_time;
  uint64_t time_since_epoch_ns = frame._system_time_ns;

  uint64_t overlay_ns = quantize_ns(time_since_epoch_ns, overlay_interval_sec);
  std::string overlay_text = format_timestamp(overlay_ns);
  write_textfile_if_changed(overlay_text_path, overlay_text);

  AVPixelFormat src_fmt = AV_PIX_FMT_YUYV422;
  if (!sws || sws_src_fmt != src_fmt || sws_src_w != res._width || sws_src_h != res._height) {
   if (sws) {
    sws_freeContext(sws);
   }
   sws = sws_getContext(res._width, res._height, src_fmt, res._width, res._height, enc_ctx->pix_fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);
   if (!sws) {
    std::cerr << "Failed to create SwsContext\n";
    return 1;
   }
   sws_src_fmt = src_fmt;
   sws_src_w = res._width;
   sws_src_h = res._height;
  }

  ff_check(av_frame_make_writable(yuv_frame), "av_frame_make_writable");

  uint8_t const* src_slices[1] = {frame._data.data()};
  int src_linesize[1] = {static_cast<int>(res._width * 2)};

  sws_scale(sws, src_slices, src_linesize, 0, res._height, yuv_frame->data, yuv_frame->linesize);

  process_ready_frame(time_since_epoch_ns, now);

  if (config._segment_duration.count() > 0 && now - start_time > config._segment_duration) {
   std::cout << "\nReached segment limit, rotating file.\n";
   break;
  }
 }

 std::cout << "\nStopping segment...\n";

 ff_check(avcodec_send_frame(enc_ctx, nullptr), "avcodec_send_frame(flush)");
 while (true) {
  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = nullptr;
  pkt.size = 0;

  int ret = avcodec_receive_packet(enc_ctx, &pkt);
  if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
   av_packet_unref(&pkt);
   break;
  }
  ff_check(ret, "avcodec_receive_packet(flush)");

  av_packet_rescale_ts(&pkt, enc_ctx->time_base, st->time_base);
  pkt.stream_index = st->index;

  ff_check(av_interleaved_write_frame(fmt_ctx, &pkt), "av_interleaved_write_frame(flush)");
  av_packet_unref(&pkt);
 }

 ff_check(av_write_trailer(fmt_ctx), "av_write_trailer");

 if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
  avio_closep(&fmt_ctx->pb);
 }

 if (filter_graph) {
  avfilter_graph_free(&filter_graph);
 }

 av_frame_free(&yuv_frame);
 if (sws) {
  sws_freeContext(sws);
 }
 avcodec_free_context(&enc_ctx);
 avformat_free_context(fmt_ctx);

 std::cout << "Total frames this segment: " << frame_count << "\n";
 return 0;
}

}  // namespace

}  // namespace pmt::cctv

using namespace pmt::cctv;

auto main(int argc, char const* const* argv) -> int {
 argh::parser cmdl;
 cmdl.parse(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

 auto print_help = []() {
  std::cout << "Usage: pmt_cctv [options]\n"
               "Options:\n"
               "  --device <path>                 Video4Linux device path (default: /dev/video2)\n"
               "  --dest-dir <path>               Root directory for recordings (default: recordings)\n"
               "  --segment-seconds <int>         Segment length in seconds (default: 3600)\n"
               "  --duration-seconds <int>        Alias for --segment-seconds\n"
               "  --retention-days <int>          Days of recordings to retain (default: 30)\n"
               "  --overlay-interval-seconds <int>Timestamp overlay update interval (default: 2)\n"
               "  --codec-name <name>             FFmpeg encoder (default: libx265)\n"
               "  --codec-preset <preset>         Encoder preset (default: medium)\n"
               "  --codec-crf <value>             Encoder CRF (default: 23)\n"
               "  --overlay-text-path <path>      Temp text file for overlay (default: /tmp/cctv_timestamp.txt)\n"
               "  -h, --help                      Show this help text\n";
 };

 if (cmdl[{"-h", "--help"}]) {
  print_help();
  return 0;
 }

 CaptureConfig config{};
 auto assign_string = [&](char const* name, std::string& field) {
  std::string value;
  if (cmdl(name) >> value) {
   field = std::move(value);
  }
 };
 auto assign_int = [&](char const* name, auto& field) {
  using FieldType = std::decay_t<decltype(field)>;
  FieldType value{};
  if (cmdl(name) >> value) {
   field = value;
  }
 };
 auto assign_seconds = [&](char const* name, std::chrono::seconds& field) {
  int64_t value = 0;
  if (cmdl(name) >> value) {
   field = std::chrono::seconds{value};
  }
 };

 assign_string("device", config._device_path);
 assign_string("dest-dir", config._dest_dir);
 assign_seconds("segment-seconds", config._segment_duration);
 assign_seconds("duration-seconds", config._segment_duration);
 assign_int("retention-days", config._retention_days);
 assign_int("overlay-interval-seconds", config._overlay_interval_sec);
 assign_string("codec-name", config._codec_name);
 assign_string("codec-preset", config._codec_preset);
 assign_string("codec-crf", config._codec_crf);
 assign_string("overlay-text-path", config._overlay_text_path);

 std::signal(SIGINT, handle_sigint);

 CameraCapture camera(config._device_path);
 CameraCapture::Resolution const res = camera.get_resolution();

 std::cout << "Recording segments... press Ctrl+C to stop.\n";

 while (g_running) {
  auto segment_start = std::chrono::system_clock::now();
  std::filesystem::path output_path = make_segment_path(segment_start, config._dest_dir);
  std::cout << "Segment output: " << output_path << "\n";

  record_segment(config, camera, res, output_path);
  prune_recordings(config._dest_dir, config._retention_days);
 }

 std::cout << "Done.\n";
 return 0;
}
