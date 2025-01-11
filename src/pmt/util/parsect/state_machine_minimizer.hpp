#pragma once

#include "pmt/util/parsect/state_machine.hpp"

namespace pmt::util::parsect {

class StateMachineMinimizer {
 public:
  static void minimize(StateMachine& state_machine_);
};

}  // namespace pmt::util::parsect
