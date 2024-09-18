#pragma once

#include <filesystem>
#include <string_view>

namespace pmt::base {

/// \note May or may not be same as std::string_view
using path_view = std::basic_string_view<std::filesystem::path::value_type>;

}  // namespace pmt::base