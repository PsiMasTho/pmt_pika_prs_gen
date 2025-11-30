#pragma once

#include <chrono>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include <linux/videodev2.h>

namespace pmt::cctv {

class CameraCapture {
public:
 using SteadyClock = std::chrono::steady_clock;

 class FrameLease;

 struct Frame {
  std::span<uint8_t> _data{};
  uint32_t _buffer_index = 0;
  uint64_t _system_time_ns = 0;
  SteadyClock::time_point _steady_time{};
 };

 struct Resolution {
  uint32_t _width = 0;
  uint32_t _height = 0;
 };

public:
 CameraCapture(std::string device_path_, size_t buffer_count_ = 8);
 CameraCapture(CameraCapture const&) = delete;
 CameraCapture(CameraCapture&&) = delete;
 auto operator=(CameraCapture const&) -> CameraCapture& = delete;
 auto operator=(CameraCapture&&) -> CameraCapture& = delete;
 ~CameraCapture();

 auto get_frame() -> FrameLease;
 auto get_resolution() const -> Resolution;

private:
 void init_device(size_t buffer_count_);
 void request_buffers(size_t buffer_count_);
 auto find_best_yuyv_resolution() -> Resolution;
 void map_buffers();
 void queue_all_buffers();
 void start_streaming();
 void stop_streaming();
 auto dequeue_frame() -> Frame;
 void queue_frame(Frame const& frame_);

 [[noreturn]] static void errno_die(const char* msg_);
 [[noreturn]] static void die(const char* msg_);

private:
 std::string _device;
 int _fd = -1;
 std::vector<Frame> _buffers;
 Resolution _resolution;
 bool _streaming = false;
};

class CameraCapture::FrameLease {
public:
 FrameLease(CameraCapture& owner_, Frame frame_);
 FrameLease(FrameLease const&) = delete;
 FrameLease(FrameLease&& other_) noexcept;
 auto operator=(FrameLease const&) -> FrameLease& = delete;
 auto operator=(FrameLease&& other_) noexcept -> FrameLease&;
 ~FrameLease();

 auto frame() const -> Frame const&;
 auto operator->() const -> Frame const*;

private:
 CameraCapture* _owner = nullptr;
 Frame _frame{};
};

}  // namespace pmt::cctv
