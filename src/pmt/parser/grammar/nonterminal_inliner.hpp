#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, GrammarData)
PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst)

namespace pmt::parser::grammar {

class NonterminalInliner {
 public:
  // -$ Types / Constants $-
  class Args {
   public:
   GrammarData& _grammar_data;
   pmt::parser::GenericAst& _ast;
  };

  // -$ Functions $-
  // --$ Other $--
  static auto do_inline(Args args_) -> size_t;
};

}