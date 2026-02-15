#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::pika::builder, PikaTables);

namespace pmt::pika::builder {

auto pika_tables_to_txt_str(PikaTables const& program_) -> std::string;

}  // namespace pmt::pika::builder