#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::builder, PikaProgram);

namespace pmt::builder {

auto pika_program_to_string(PikaProgram const& program_) -> std::string;

}  // namespace pmt::builder