#pragma once

#include <string>
#include <string_view>

namespace pmt::util {

void set_skeleton_root_override(std::string root_);
auto get_skeleton_path(std::string_view relative_) -> std::string;

}  // namespace pmt::util
