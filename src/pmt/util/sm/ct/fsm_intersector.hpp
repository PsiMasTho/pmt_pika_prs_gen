#pragma once

#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/primitives.hpp"

namespace pmt::util::sm::ct {

class FsmIntersector {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  StateMachine _state_machine_lhs;
  StateMachine _state_machine_rhs;
  StateNrType _state_nr_from_lhs = StateNrStart;
  StateNrType _state_nr_from_rhs = StateNrStart;
 };

 // -$ Functions $-
 // --$ Other $--
 static auto intersect(Args args_) -> StateMachine;
};

}  // namespace pmt::util::sm::ct