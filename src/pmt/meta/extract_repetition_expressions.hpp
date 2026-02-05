#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::ast, Ast)

namespace pmt::meta {

void extract_repetition_expressions(pmt::ast::Ast& ast_);

}  // namespace pmt::meta
