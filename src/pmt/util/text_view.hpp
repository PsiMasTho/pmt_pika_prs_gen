#pragma once

#include <cstddef>
#include <string_view>

#include "pmt/util/text_encoding.hpp"

namespace pmt::util {

class text_view {
 public:
  using codepoint_type = char32_t;
  using unsigned_codepoint_type = std::make_unsigned_t<codepoint_type>;
  using fat_codepoint_type = std::pair<codepoint_type, uint32_t>;
  using fat_unsigned_codepoint_type = std::pair<unsigned_codepoint_type, uint32_t>;

  static const codepoint_type INVALID_CODEPOINT;

  template <typename CHAR_TYPE_>
  text_view(std::basic_string_view<CHAR_TYPE_> str_, text_encoding encoding_);

  text_view(unsigned char const* begin_, unsigned char const* end_, text_encoding encoding_);

  auto get_position() const -> ptrdiff_t;
  auto set_position(ptrdiff_t position_) -> void;

  /// Returns the maximum number of codepoints that could still be read.
  /// The actual amount may be less if the byte width is > 1.
  auto get_max_remaining() const -> ptrdiff_t;
  auto get_bytes_remaining() const -> ptrdiff_t;

  auto read() -> codepoint_type;
  auto is_at_end() const -> bool;

  auto get_encoding() const -> text_encoding;

 private:
  unsigned char const* _begin;
  unsigned char const* _cursor;
  unsigned char const* _end;

  fat_codepoint_type (*_read_fn)(unsigned char const*, unsigned char const*);
  fat_codepoint_type (*_conversion_fn)(fat_codepoint_type);

  text_encoding _encoding;
};

}  // namespace pmt::util

#include "pmt/util/text_view-inl.hpp"