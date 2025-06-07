#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::parserbuilder {

class ParserNonterminalInliner {
 public:
  // -$ Types / Constants $-
  class Args {
   public:
   pmt::base::IntervalSet<pmt::util::smrt::AcceptsIndexType> _nonterminals_to_inline;
   pmt::util::smct::StateMachine& _parser_state_machine;
  };

  // -$ Functions $-
  // --$ Other $--
  static void unpack(Args args_);
};

}