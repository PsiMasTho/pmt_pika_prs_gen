#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, GrammarData)
PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst)

namespace pmt::parserbuilder {

class NonterminalInliner {
 public:
  // -$ Types / Constants $-
  class Args {
   public:
   GrammarData& _grammar_data;
   pmt::util::smrt::GenericAst& _ast;
  };

  // -$ Functions $-
  // --$ Other $--
  static auto do_inline(Args args_) -> size_t;
};

}