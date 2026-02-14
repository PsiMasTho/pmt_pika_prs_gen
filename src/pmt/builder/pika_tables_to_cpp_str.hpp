#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::builder, PikaTables);

namespace pmt::builder {

static inline char const pika_tables_hdr_skel_path_rel[] = "cpp/pika_tables-skel.hpp";
static inline char const pika_tables_src_skel_path_rel[] = "cpp/pika_tables-skel.cpp";

auto pika_tables_to_cpp_str_hdr(std::string const& class_name_, std::string const& namespace_name_, std::string header_skel_) -> std::string;
auto pika_tables_to_cpp_str_src(PikaTables const& pika_tables_, std::string const& class_name_, std::string const& namespace_name_, std::string const& header_include_path_, std::string const& id_constants_include_path_, std::string source_skel_) -> std::string;

}  // namespace pmt::builder
