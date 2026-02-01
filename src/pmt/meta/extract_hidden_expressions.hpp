#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::rt, Ast)

namespace pmt::meta {

void extract_hidden_expressions(pmt::rt::Ast& ast_);

}  // namespace pmt::meta