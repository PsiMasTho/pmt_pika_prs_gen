#pragma once

#include <string>

namespace pmt::util {

auto read_file(std::string_view file_path_) -> std::string;

}