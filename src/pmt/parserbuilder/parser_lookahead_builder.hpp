#pragma once

#include "pmt/util/smct/state_machine.hpp"

#include <functional>

namespace pmt::parserbuilder {

class ParserLookaheadBuilder {
 public:
  // -$ Types / Constants $-
  using LabelLookupFn = std::function<std::string(size_t)>;

  class Args {
   public:
   pmt::util::smct::StateMachine& _parser_state_machine;
   pmt::base::IntervalSet<pmt::util::smrt::StateNrType> _conflicting_state_nrs; // returned by extract_conflicts
   LabelLookupFn _fn_lookup_terminal_label;
  };

  // -$ Functions $-
  // --$ Other $--
  static auto build(Args args_) -> pmt::util::smct::StateMachine;
  static auto extract_conflicts(pmt::util::smct::StateMachine& state_machine_) -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;
};

}