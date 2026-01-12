#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/grammar/grammar.hpp"

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst)

namespace pmt::parser::grammar {

class GrammarFromAst2 {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  GenericAst const& _ast;
 };

 // -$ Functions $-
 // --$ Other $--
 static auto make(Args args_) -> Grammar;
};

}  // namespace pmt::parser::grammar