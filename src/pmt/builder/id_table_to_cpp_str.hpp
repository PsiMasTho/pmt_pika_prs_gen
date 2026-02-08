#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::meta, IdTable);

namespace pmt::builder {

auto id_table_constants_to_cpp_str(pmt::meta::IdTable const& id_table_, std::string id_constants_skel_) -> std::string;
auto id_table_strings_to_cpp_str(pmt::meta::IdTable const& id_table_, std::string id_strings_skel_) -> std::string;

}  // namespace pmt::builder
