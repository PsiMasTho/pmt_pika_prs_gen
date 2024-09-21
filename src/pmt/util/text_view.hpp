#pragma once

#include <cstddef>
#include <span>
#include <string_view>

#include "pmt/util/text_encoding.hpp"

namespace pmt::util {

class text_view {
 public:
  using codepoint_type = char32_t;
  static const codepoint_type INVALID_CODEPOINT;

  template <typename CHAR_TYPE_>
  text_view(std::basic_string_view<CHAR_TYPE_> str_, text_encoding encoding_);

  auto get_position() const -> size_t;
  auto set_position(size_t position_) -> void;

  /// Returns the maximum number of codepoints that could still be read.
  /// The actual amount may be less if the byte width is > 1.
  auto get_max_remaining() const -> size_t;
  auto get_bytes_remaining() const -> size_t;

  auto read() -> codepoint_type;
  auto is_at_end() const -> bool;

  auto get_encoding() const -> text_encoding;

 private:
  std::span<std::byte const> _data;
  size_t _position;
  text_encoding _encoding;
};

}  // namespace pmt::util

#include "pmt/util/text_view-inl.hpp"