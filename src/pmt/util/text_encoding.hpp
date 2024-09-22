#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

namespace pmt::util {

enum text_encoding : std::uint8_t {
  //-- Masks --
  MASK_CODE_UNIT_SIZE = 0b0000'0111,
  MASK_ENDIAN = 0b0000'1000,
  //-- Endian --
  VALUE_LITTLE_ENDIAN = 0b0000'0000,
  VALUE_BIG_ENDIAN = 0b0000'1000,
  VALUE_NATIVE_ENDIAN = std::endian::native == std::endian::little ? VALUE_LITTLE_ENDIAN : VALUE_BIG_ENDIAN,
  //-- Code unit size --
  VALUE_CODE_UNIT_SIZE_1 = 0b0000'0001,
  VALUE_CODE_UNIT_SIZE_2 = 0b0000'0010,
  VALUE_CODE_UNIT_SIZE_4 = 0b0000'0100,
  //-- Encodings (native) --
  ENCODING_UTF8 = VALUE_NATIVE_ENDIAN | VALUE_CODE_UNIT_SIZE_1,
  ENCODING_UTF16 = VALUE_NATIVE_ENDIAN | VALUE_CODE_UNIT_SIZE_2,
  ENCODING_UTF32 = VALUE_NATIVE_ENDIAN | VALUE_CODE_UNIT_SIZE_4,
  //-- Encodings (little endian) --
  ENCODING_UTF16LE = VALUE_LITTLE_ENDIAN | VALUE_CODE_UNIT_SIZE_2,
  ENCODING_UTF32LE = VALUE_LITTLE_ENDIAN | VALUE_CODE_UNIT_SIZE_4,
  //-- Encodings (big endian) --
  ENCODING_UTF16BE = VALUE_BIG_ENDIAN | VALUE_CODE_UNIT_SIZE_2,
  ENCODING_UTF32BE = VALUE_BIG_ENDIAN | VALUE_CODE_UNIT_SIZE_4,
};

//-- Types ---------------------------------------------------------------------
using utf32_type = char32_t;
using utf16_type = char16_t;
using utf8_type = char8_t;

using codepoint_type = utf32_type;

//-- Traits --------------------------------------------------------------------
template <text_encoding ENCODING_>
struct text_encoding_traits {
  static constexpr bool IS_NATIVE_ENDIAN = (ENCODING_ & MASK_ENDIAN) == VALUE_NATIVE_ENDIAN;
  static constexpr bool IS_LITTLE_ENDIAN = (ENCODING_ & MASK_ENDIAN) == VALUE_LITTLE_ENDIAN;
  static constexpr bool IS_BIG_ENDIAN = (ENCODING_ & MASK_ENDIAN) == VALUE_BIG_ENDIAN;

  static constexpr size_t CODE_UNIT_SIZE = (ENCODING_ & MASK_CODE_UNIT_SIZE) == VALUE_CODE_UNIT_SIZE_1 ? 1 : (ENCODING_ & MASK_CODE_UNIT_SIZE) == VALUE_CODE_UNIT_SIZE_2 ? 2 : 4;

  using code_unit_type = std::conditional_t<CODE_UNIT_SIZE == 1, utf8_type, std::conditional_t<CODE_UNIT_SIZE == 2, utf16_type, utf32_type>>;
  using string_view_type = std::basic_string_view<code_unit_type>;
  using string_type = std::basic_string<code_unit_type>;
};

//-- Special codepoints --------------------------------------------------------
const codepoint_type INVALID_CODEPOINT = std::numeric_limits<codepoint_type>::max();

}  // namespace pmt::util