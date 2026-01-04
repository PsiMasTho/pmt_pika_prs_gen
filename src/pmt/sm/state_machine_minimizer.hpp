#pragma once

#include "pmt/sm/state_machine.hpp"

namespace pmt::sm {

class StateMachineMinimizer {
public:
 // -$ Functions $-
 // --$ Other $--
 static auto minimize(StateMachine const& input_state_machine_) -> StateMachine;
};

}  // namespace pmt::sm
