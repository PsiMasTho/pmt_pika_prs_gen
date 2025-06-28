#include "pmt/util/sm/ct/fsm_intersector.hpp"

#include "pmt/util/sm/ct/state_machine_determinizer.hpp"

namespace pmt::util::sm::ct {

class Locals {
public:
 StateMachine _state_machine_ret;
};

auto FsmIntersector::intersect(Args args_) -> StateMachine {
 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = args_._state_machine_lhs, ._state_nr_from = args_._state_nr_from_lhs});
 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = args_._state_machine_rhs, ._state_nr_from = args_._state_nr_from_rhs});

 Locals locals;

 return locals._state_machine_ret;
}

}  // namespace pmt::util::sm::ct