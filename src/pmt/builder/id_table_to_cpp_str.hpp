#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::meta, IdTable);

namespace pmt::builder {

static inline char const id_constants_skel_path_rel[] = "cpp/id_constants-skel.hpp";
static inline char const id_strings_skel_path_rel[] = "cpp/id_strings-skel.hpp";

auto id_table_constants_to_cpp_str(pmt::meta::IdTable const& id_table_, std::string id_constants_skel_) -> std::string;
auto id_table_strings_to_cpp_str(pmt::meta::IdTable const& id_table_, std::string id_strings_skel_) -> std::string;

}  // namespace pmt::builder
