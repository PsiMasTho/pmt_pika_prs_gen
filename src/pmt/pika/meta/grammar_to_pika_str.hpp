#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::pika::meta, Grammar);

namespace pmt::pika::meta {

auto grammar_to_pika_str(Grammar const& grammar_) -> std::string;

}  // namespace pmt::pika::meta