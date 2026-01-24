#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

namespace pmt::builder {

auto read_stream(std::istream& stream_, std::string_view label_) -> std::string;

}  // namespace pmt::builder
