#include "pmt/util/text_cursor.hpp"

#include "pmt/asserts.hpp"
#include "pmt/util/text_encoding.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace pmt::util {
namespace {
using BufferType = std::array<unsigned char, 4>;
const TextCursor::FatCodepointType INVALID_FAT_CODEPOINT = {INVALID_CODEPOINT, 0};

using buffer16_type = std::array<uint16_t, 2>;

auto to_buffer(CodepointType codepoint_) -> BufferType {
  return std::bit_cast<BufferType>(codepoint_);
}

auto from_buffer(BufferType buffer_) -> CodepointType {
  return std::bit_cast<CodepointType>(buffer_);
}

auto to_buffer16(CodepointType codepoint_) -> buffer16_type {
  return std::bit_cast<buffer16_type>(codepoint_);
}

// There are neater ways to do this, but this is the fastest i found
auto calculate_utf8_length(unsigned char lsb_) -> size_t {
  if ((lsb_ & 0x80) == 0) [[likely]]  // tuned for ASCII
    return 1;

  size_t const countl = std::countl_one(lsb_);

  if (countl < 5) [[likely]]
    return countl;

  return 0;
}

auto convert_utf8(TextCursor::FatCodepointType codepoint_) -> TextCursor::FatCodepointType {
  static std::array<UnsignedCodepointType, 4> const first_masks = {0x7F, 0x1F, 0x0F, 0x07};
  static UnsignedCodepointType const continuation_mask = 0x3F;

  static size_t const continuation_shift = 6;
  static std::array<size_t, 4> const first_shifts = {0, 6, 12, 18};

  BufferType const buffer = to_buffer(codepoint_.first);
  size_t const length = calculate_utf8_length(buffer[0]);

  if (length > codepoint_.second) [[unlikely]]
    return INVALID_FAT_CODEPOINT;

  // set the first
  UnsignedCodepointType result = 0ULL | ((buffer.front() & first_masks[length - 1]) << first_shifts[length - 1]);

  // set the rest
  for (size_t i = 1; i < length; ++i) {
    if ((buffer[i] & 0xC0) != 0x80) [[unlikely]]
      return INVALID_FAT_CODEPOINT;

    UnsignedCodepointType continuation = buffer[i] & continuation_mask;
    size_t const shift = first_shifts[length - 1] - continuation_shift * i;

    result |= continuation << shift;
  }

  return {result, length};
}

auto convert_utf16(TextCursor::FatCodepointType codepoint_) -> TextCursor::FatCodepointType {
  buffer16_type const buffer = to_buffer16(codepoint_.first);
  uint32_t c1 = buffer[0];
  if (c1 >= 0xD800 && c1 <= 0xDBFF) {
    if (codepoint_.second < 2) [[unlikely]]
      return INVALID_FAT_CODEPOINT;

    uint32_t c2 = buffer[1];
    if (c2 < 0xDC00 || c2 > 0xDFFF) [[unlikely]]
      return INVALID_FAT_CODEPOINT;

    c1 = (c1 - 0xD800) << 10;
    c2 = c2 - 0xDC00;
    return {0x10000 + c1 + c2, 4};
  }

  return {c1, 2};
}

template <size_t WIDTH_, bool IS_NATIVE_ENDIAN_>
auto read_fn(unsigned char const* cursor_, unsigned char const* end_) -> TextCursor::FatCodepointType {
  static_assert(WIDTH_ == 1 || WIDTH_ == 2 || WIDTH_ == 4);

  size_t const remaining = std::distance(cursor_, end_);
  if (remaining < WIDTH_ || remaining % WIDTH_ != 0) [[unlikely]]
    return INVALID_FAT_CODEPOINT;

  TextCursor::FatCodepointType result{0, std::min(4UL, remaining)};
  std::memcpy(&result.first, cursor_, result.second);

  if constexpr (!IS_NATIVE_ENDIAN_ && WIDTH_ != 1) {
    BufferType buffer = to_buffer(result.first);
    if constexpr (WIDTH_ == 4) {
      std::ranges::reverse(buffer);
    } else if constexpr (WIDTH_ == 2) {
      std::swap(buffer[0], buffer[1]);
      std::swap(buffer[2], buffer[3]);
    }
    result.first = from_buffer(buffer);
  }

  return result;
}

template <size_t WIDTH_>
auto conversion_fn(TextCursor::FatCodepointType codepoint_) -> TextCursor::FatCodepointType {
  static_assert(WIDTH_ == 1 || WIDTH_ == 2 || WIDTH_ == 4);

  if constexpr (WIDTH_ == 1)
    return convert_utf8(codepoint_);
  else if constexpr (WIDTH_ == 2)
    return convert_utf16(codepoint_);
  else if constexpr (WIDTH_ == 4)
    return codepoint_;
  else
    pmt_unreachable();
}

}  // namespace

TextCursor::TextCursor(std::string_view str_, TextEncoding encoding_)
 : _begin(reinterpret_cast<unsigned char const*>(str_.data()))
 , _cursor(_begin)
 , _end(_begin + str_.size())
 , _read_fn(nullptr)
 , _conversion_fn(nullptr)
 , _encoding(encoding_) {
  // Bind the functions
  bool const is_native_endian = (encoding_ & MASK_ENDIAN) == VALUE_NATIVE_ENDIAN;
  size_t const code_unit_size = (encoding_ & MASK_CODE_UNIT_SIZE);
  switch (code_unit_size) {
    case 1:
      _read_fn = is_native_endian ? read_fn<1, true> : read_fn<1, false>;
      _conversion_fn = conversion_fn<1>;
      break;
    case 2:
      _read_fn = is_native_endian ? read_fn<2, true> : read_fn<2, false>;
      _conversion_fn = conversion_fn<2>;
      break;
    case 4:
      _read_fn = is_native_endian ? read_fn<4, true> : read_fn<4, false>;
      _conversion_fn = conversion_fn<4>;
      break;
    default:
      pmt_unreachable();
  }
}

auto TextCursor::get_position() const -> ptrdiff_t {
  return std::distance(_begin, _cursor);
}

auto TextCursor::set_position(ptrdiff_t position_) -> void {
  assert(position_ <= std::distance(_begin, _end));
  _cursor = std::next(_begin, position_);
}

auto TextCursor::get_max_remaining() const -> ptrdiff_t {
  return get_bytes_remaining() / (_encoding & MASK_CODE_UNIT_SIZE);
}

auto TextCursor::get_bytes_remaining() const -> ptrdiff_t {
  return std::distance(_cursor, _end);
}

auto TextCursor::read() -> CodepointType {
  FatCodepointType const result = _conversion_fn(_read_fn(_cursor, _end));
  _cursor = (result.first == INVALID_CODEPOINT) ? _end : std::next(_cursor, result.second);
  return result.first;
}

auto TextCursor::is_at_end() const -> bool {
  return get_bytes_remaining() == 0;
}

auto TextCursor::get_encoding() const -> TextEncoding {
  return _encoding;
}

}  // namespace pmt::util