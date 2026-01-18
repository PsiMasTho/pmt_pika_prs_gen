#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

namespace pmt::builder {

auto read_stream(std::istream& file_, std::string_view label_) -> std::string;
auto format_hex(uint64_t value_, size_t width_ = 0) -> std::string;

}  // namespace pmt::builder
