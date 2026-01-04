#pragma once

#include "pmt/sm/state_machine.hpp"

namespace pmt::sm {

class StateMachineDeterminizer {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  StateMachine const& _input_state_machine;
  StateNrType _state_nr_from = StateNrStart;
 };

 // -$ Functions $-
 // --$ Other $--
 static auto determinize(Args args_) -> StateMachine;
};

}  // namespace pmt::sm