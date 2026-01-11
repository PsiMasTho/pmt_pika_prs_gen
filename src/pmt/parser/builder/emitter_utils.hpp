#pragma once

#include <fstream>
#include <string>
#include <string_view>

namespace pmt::parser::builder {

auto read_stream(std::ifstream& file_, std::string_view label_) -> std::string;
auto format_hex(uint64_t value_, size_t width_ = 0) -> std::string;

}  // namespace pmt::parser::builder
