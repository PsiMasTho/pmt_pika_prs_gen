#include "pmt/cctv/camera_capture.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <utility>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace pmt::cctv {

namespace {

auto now_ns() -> uint64_t {
 auto const now = std::chrono::system_clock::now();
 return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
}

}  // namespace

CameraCapture::CameraCapture(std::string device_path_, size_t buffer_count_)
 : _device(std::move(device_path_))
 , _fd(::open(_device.c_str(), O_RDWR)) {
 if (_fd == -1) {
  errno_die("Failed to open video device");
 }

 init_device(buffer_count_);
}

CameraCapture::~CameraCapture() {
 if (_streaming) {
  stop_streaming();
 }

 for (auto& b : _buffers) {
  if (!b._data.empty()) {
   munmap(b._data.data(), b._data.size());
  }
 }

 if (_fd != -1) {
  close(_fd);
 }
}

auto CameraCapture::get_frame() -> FrameLease {
 return FrameLease(*this, dequeue_frame());
}

auto CameraCapture::get_resolution() const -> Resolution {
 return _resolution;
}

auto CameraCapture::dequeue_frame() -> Frame {
 v4l2_buffer buf{};
 buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 buf.memory = V4L2_MEMORY_MMAP;

 while (true) {
  if (ioctl(_fd, VIDIOC_DQBUF, &buf) == -1) {
   if (errno == EINTR) {
    continue;
   }
   errno_die("VIDIOC_DQBUF");
  }

  if (buf.flags & V4L2_BUF_FLAG_ERROR) {
   std::cerr << "Warning: V4L2 buffer reported an error, skipping frame\n";
   queue_frame(_buffers[buf.index]);
   continue;
  }

  Frame frame = _buffers[buf.index];
  frame._data = frame._data.first(static_cast<size_t>(buf.bytesused));
  frame._system_time_ns = now_ns();
  frame._steady_time = SteadyClock::now();
  return frame;
 }
}

void CameraCapture::queue_frame(Frame const& frame_) {
 v4l2_buffer buf{};
 buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 buf.memory = V4L2_MEMORY_MMAP;
 buf.index = frame_._buffer_index;

 if (ioctl(_fd, VIDIOC_QBUF, &buf) == -1) {
  errno_die("VIDIOC_QBUF");
 }
}

void CameraCapture::init_device(size_t buffer_count_) {
 v4l2_capability cap{};
 if (ioctl(_fd, VIDIOC_QUERYCAP, &cap) == -1) {
  errno_die("VIDIOC_QUERYCAP");
 }

 if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
  die("Device does not support video capture");
 }

 if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
  die("Device does not support streaming I/O");
 }

 _resolution = find_best_yuyv_resolution();

 v4l2_format fmt{};
 fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 fmt.fmt.pix.width = _resolution._width;
 fmt.fmt.pix.height = _resolution._height;
 fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
 fmt.fmt.pix.field = V4L2_FIELD_ANY;

 if (ioctl(_fd, VIDIOC_S_FMT, &fmt) == -1) {
  errno_die("VIDIOC_S_FMT");
 }

 if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
  die("Device does not support YUYV pixel format");
 }

 v4l2_streamparm parm{};
 parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 if (ioctl(_fd, VIDIOC_G_PARM, &parm) == 0) {
  parm.parm.capture.timeperframe.numerator = 1;
  parm.parm.capture.timeperframe.denominator = 30;
  if (ioctl(_fd, VIDIOC_S_PARM, &parm) == -1) {
   std::cerr << "Warning: VIDIOC_S_PARM failed: " << std::strerror(errno) << "\n";
  } else if (ioctl(_fd, VIDIOC_G_PARM, &parm) == 0) {
   std::cout << "Driver timeperframe: " << parm.parm.capture.timeperframe.numerator << "/" << parm.parm.capture.timeperframe.denominator << " seconds per frame\n";
  }
 }

 request_buffers(buffer_count_);
 map_buffers();
 queue_all_buffers();
 start_streaming();
}

void CameraCapture::request_buffers(size_t buffer_count_) {
 v4l2_requestbuffers req{};
 req.count = buffer_count_;
 req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 req.memory = V4L2_MEMORY_MMAP;

 if (ioctl(_fd, VIDIOC_REQBUFS, &req) == -1) {
  errno_die("VIDIOC_REQBUFS");
 }

 if (req.count < 2) {
  die("Insufficient buffer memory");
 }

 _buffers.resize(req.count);
}

auto CameraCapture::find_best_yuyv_resolution() -> Resolution {
 Resolution ret{._width = 0, ._height = 0};

 v4l2_fmtdesc fmtdesc{};
 fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 fmtdesc.index = 0;
 while (ioctl(_fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
  if (fmtdesc.pixelformat == V4L2_PIX_FMT_YUYV) {
   v4l2_frmsizeenum frmsize{};
   frmsize.pixel_format = fmtdesc.pixelformat;
   frmsize.index = 0;
   while (ioctl(_fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0) {
    if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
     uint32_t area = frmsize.discrete.width * frmsize.discrete.height;
     uint32_t best_area = ret._width * ret._height;
     if (area > best_area) {
      ret._width = frmsize.discrete.width;
      ret._height = frmsize.discrete.height;
     }
    }
    frmsize.index++;
   }
  }
  fmtdesc.index++;
 }

 if (ret._width == 0 || ret._height == 0) {
  die("Failed to find any YUYV resolutions");
 }

 return ret;
}

void CameraCapture::map_buffers() {
 for (size_t i = 0; i < _buffers.size(); ++i) {
  v4l2_buffer buf{};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = static_cast<uint32_t>(i);

  if (ioctl(_fd, VIDIOC_QUERYBUF, &buf) == -1) {
   errno_die("VIDIOC_QUERYBUF");
  }

  void* ptr = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, buf.m.offset);

  if (ptr == MAP_FAILED) {
   errno_die("mmap");
  }
  _buffers[i]._data = std::span<uint8_t>(static_cast<uint8_t*>(ptr), buf.length);
  _buffers[i]._buffer_index = buf.index;
 }
}

void CameraCapture::queue_all_buffers() {
 for (auto const& buffer : _buffers) {
  v4l2_buffer buf{};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = buffer._buffer_index;
  if (ioctl(_fd, VIDIOC_QBUF, &buf) == -1) {
   errno_die("VIDIOC_QBUF");
  }
 }
}

void CameraCapture::start_streaming() {
 v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 if (ioctl(_fd, VIDIOC_STREAMON, &type) == -1) {
  errno_die("VIDIOC_STREAMON");
 }
 _streaming = true;
}

void CameraCapture::stop_streaming() {
 v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 if (ioctl(_fd, VIDIOC_STREAMOFF, &type) == -1) {
  errno_die("VIDIOC_STREAMOFF");
 }
 _streaming = false;
}

[[noreturn]] void CameraCapture::errno_die(const char* msg_) {
 std::cerr << msg_ << ": " << std::strerror(errno) << "\n";
 std::exit(EXIT_FAILURE);
}

[[noreturn]] void CameraCapture::die(const char* msg_) {
 std::cerr << msg_ << "\n";
 std::exit(EXIT_FAILURE);
}

CameraCapture::FrameLease::FrameLease(CameraCapture& owner_, Frame frame_)
 : _owner(&owner_)
 , _frame(frame_) {
}

CameraCapture::FrameLease::FrameLease(FrameLease&& other_) noexcept
 : _owner(std::exchange(other_._owner, nullptr))
 , _frame(std::exchange(other_._frame, Frame{})) {
}

auto CameraCapture::FrameLease::operator=(FrameLease&& other_) noexcept -> FrameLease& {
 if (this == &other_) {
  return *this;
 }

 if (_owner) {
  _owner->queue_frame(_frame);
 }

 _owner = std::exchange(other_._owner, nullptr);
 _frame = std::exchange(other_._frame, Frame{});
 return *this;
}

CameraCapture::FrameLease::~FrameLease() {
 if (_owner) {
  _owner->queue_frame(_frame);
 }
}

auto CameraCapture::FrameLease::frame() const -> Frame const& {
 return _frame;
}

auto CameraCapture::FrameLease::operator->() const -> Frame const* {
 return &_frame;
}

}  // namespace pmt::cctv
