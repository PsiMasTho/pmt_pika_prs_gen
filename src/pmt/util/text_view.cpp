#include "pmt/util/text_view.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "pmt/asserts.hpp"
#include "pmt/util/endianness.hpp"
#include "pmt/util/text_encoding.hpp"

namespace pmt::util {
namespace {
using buffer_type = std::array<std::byte, 4>;
using unsigned_codepoint_type = std::make_unsigned_t<text_view::codepoint_type>;

auto to_buffer(text_view::codepoint_type codepoint_) -> buffer_type {
  return std::bit_cast<buffer_type>(codepoint_);
}

auto from_buffer(buffer_type buffer_) -> text_view::codepoint_type {
  return std::bit_cast<text_view::codepoint_type>(buffer_);
}

auto unsigned_from_buffer(buffer_type buffer_) -> unsigned_codepoint_type {
  return std::bit_cast<unsigned_codepoint_type>(buffer_);
}

auto read_codepoint(size_t& read_, std::span<std::byte const> subspan_) -> buffer_type {
  size_t const count = std::min(subspan_.size(), 4UL);
  buffer_type buffer = to_buffer(0);
  std::copy_n(subspan_.begin(), count, buffer.begin());
  read_ = count;
  return buffer;
}

// There are neater ways to do this, but this is the fastest i found
auto calculate_utf8_length(std::byte lsb_) -> size_t {
  auto lsb = std::to_integer<uint8_t>(lsb_);

  if ((lsb & 0x80) == 0) [[likely]]  // tuned for ASCII
    return 1;

  size_t const countl = std::countl_one(lsb);

  if (countl < 5) [[likely]]
    return countl;

  return 0;
}

auto convert_utf8(buffer_type& buffer_, size_t bytes_read_) -> size_t {
  static std::array<unsigned_codepoint_type, 4> const first_masks = {0x7F, 0x1F, 0x0F, 0x07};
  static unsigned_codepoint_type const continuation_mask = 0x3F;

  static size_t const continuation_shift = 6;
  static std::array<size_t, 4> const first_shifts = {0, 6, 12, 18};

  size_t const length = calculate_utf8_length(buffer_[0]);

  if (length > bytes_read_) {
    buffer_ = to_buffer(text_view::INVALID_CODEPOINT);
    return 0;
  }

  // set the first
  unsigned_codepoint_type const buffer = unsigned_from_buffer(buffer_);
  unsigned_codepoint_type result = 0ULL | ((buffer & first_masks[length - 1]) << first_shifts[length - 1]);

  // set the rest
  for (size_t i = 1; i < length; ++i) {
    if ((buffer_[i] & std::byte{0xC0}) != std::byte{0x80}) {
      buffer_ = to_buffer(text_view::INVALID_CODEPOINT);
      return 0;
    }

    unsigned_codepoint_type continuation = std::to_integer<unsigned_codepoint_type>(buffer_[i]) & continuation_mask;
    size_t const shift = first_shifts[length - 1] - continuation_shift * i;

    result |= continuation << shift;
  }

  buffer_ = to_buffer(result);
  return length;
}

auto convert_utf16(buffer_type& buffer_, size_t bytes_read_) -> size_t {
  return 0;
}

auto convert(buffer_type& buffer_, text_encoding encoding_, size_t bytes_read_) -> size_t {
  switch (encoding_._byte_count) {
    case 1:
      return convert_utf8(buffer_, bytes_read_);
    case 2:
      if (encoding_._endian != endianness::NATIVE) {
        std::swap(buffer_[0], buffer_[1]);
        std::swap(buffer_[2], buffer_[3]);
      }
      return convert_utf16(buffer_, bytes_read_);
    case 4:
      if (encoding_._endian != endianness::NATIVE)
        std::ranges::reverse(buffer_);
      return 4;
    default:
      pmt_unreachable();
  }
}

}  // namespace

const text_view::codepoint_type text_view::INVALID_CODEPOINT = std::numeric_limits<codepoint_type>::max();

auto text_view::get_position() const -> size_t {
  return _position;
}

auto text_view::set_position(size_t position_) -> void {
  assert(position_ <= _data.size());
  _position = position_;
}

auto text_view::get_max_remaining() const -> size_t {
  return get_bytes_remaining() / _encoding._byte_count;
}

auto text_view::get_bytes_remaining() const -> size_t {
  return std::distance(_data.begin() + _position, _data.end());
}

auto text_view::read() -> codepoint_type {
  size_t bytes_read = 0;
  buffer_type buffer = read_codepoint(bytes_read, _data.subspan(_position));

  if (bytes_read < _encoding._byte_count) {
    set_position(_data.size());
    return INVALID_CODEPOINT;
  }

  _position += convert(buffer, _encoding, bytes_read);

  codepoint_type const result = from_buffer(buffer);

  if (result == INVALID_CODEPOINT)
    set_position(_data.size());

  return result;
}

auto text_view::is_at_end() const -> bool {
  return get_bytes_remaining() == 0;
}

auto text_view::get_encoding() const -> text_encoding {
  return _encoding;
}

}  // namespace pmt::util