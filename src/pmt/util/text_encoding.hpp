#pragma once

#include <cstdint>

namespace pmt::util {

struct text_encoding {
  std::uint8_t _endian : 1;
  std::uint8_t _is_ascii : 1;
  std::uint8_t _byte_count : 3;
};

//-- Endianness independent
extern const text_encoding ASCII;
extern const text_encoding UTF8;
//-- Native endian
extern const text_encoding UTF16;
extern const text_encoding UTF32;
//-- Big endian
extern const text_encoding UTF16BE;
extern const text_encoding UTF32BE;
//-- Little endian
extern const text_encoding UTF16LE;
extern const text_encoding UTF32LE;

}  // namespace pmt::util