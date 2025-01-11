#pragma once

#include "pmt/util/parsect/state_machine.hpp"
#include "pmt/util/parsect/state_tag.hpp"

namespace pmt::util::parsect {

template <IsStateTag TAG_>
class StateMachineMinimizer {
 public:
  static void minimize(StateMachine<TAG_>& state_machine_);
};

}  // namespace pmt::util::parsect

#include "pmt/util/parsect/state_machine_minimizer-inl.hpp"