#pragma once

#include "pmt/util/text_encoding.hpp"

#include <cstddef>
#include <string_view>
#include <utility>

namespace pmt::util {

class TextCursor {
 public:
  using FatCodepointType = std::pair<CodepointType, uint32_t>;
  using FatUnsignedCodepointType = std::pair<UnsignedCodepointType, uint32_t>;

  TextCursor(std::string_view str_, TextEncoding encoding_);

  auto get_position() const -> ptrdiff_t;
  auto set_position(ptrdiff_t position_) -> void;

  /// Returns the maximum number of codepoints that could still be read.
  /// The actual amount may be less if the byte width is > 1.
  auto get_max_remaining() const -> ptrdiff_t;
  auto get_bytes_remaining() const -> ptrdiff_t;

  auto read() -> CodepointType;
  auto is_at_end() const -> bool;

  auto get_encoding() const -> TextEncoding;

 private:
  unsigned char const* _begin;
  unsigned char const* _cursor;
  unsigned char const* _end;

  FatCodepointType (*_read_fn)(unsigned char const*, unsigned char const*);
  FatCodepointType (*_conversion_fn)(FatCodepointType);

  TextEncoding _encoding;
};

}  // namespace pmt::util