#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/meta/grammar.hpp"

PMT_FW_DECL_NS_CLASS(pmt::rt, Ast)

namespace pmt::meta {

class GrammarFromAst {
public:
 // --$ Other $--
 static auto make(pmt::rt::Ast const& ast_) -> Grammar;
};

}  // namespace pmt::meta