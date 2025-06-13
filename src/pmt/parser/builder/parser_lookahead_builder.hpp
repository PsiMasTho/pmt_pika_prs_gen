#pragma once

#include "pmt/util/sm/ct/state_machine.hpp"

#include <functional>

namespace pmt::parser::builder {

class ParserLookaheadBuilder {
 public:
  // -$ Types / Constants $-
  using LabelLookupFn = std::function<std::string(size_t)>;

  class Args {
   public:
    pmt::util::sm::ct::StateMachine& _parser_state_machine;
    pmt::base::IntervalSet<pmt::util::sm::StateNrType> _conflicting_state_nrs;  // returned by extract_conflicts
    LabelLookupFn _fn_lookup_terminal_label;
  };

  // -$ Functions $-
  // --$ Other $--
  static auto build(Args args_) -> pmt::util::sm::ct::StateMachine;
  static auto extract_conflicts(pmt::util::sm::ct::StateMachine& state_machine_) -> pmt::base::IntervalSet<pmt::util::sm::StateNrType>;
};

}  // namespace pmt::parser::builder