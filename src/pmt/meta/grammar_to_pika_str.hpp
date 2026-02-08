#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::meta, Grammar);

namespace pmt::meta {

auto grammar_to_pika_str(Grammar const& grammar_) -> std::string;

}  // namespace pmt::meta