#pragma once

#include "pmt/util/parsect/state_machine.hpp"

namespace pmt::util::parsect {

class StateMachinePruner {
 public:
  template <IsStateTag TAG_>
  static void prune(StateMachine<TAG_>& state_machine_, State::StateNrType state_nr_from_ = 0, State::StateNrType state_nr_from_new_ = 0, bool renumber_ = false);
};

}  // namespace pmt::util::parsect