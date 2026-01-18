#pragma once

#include "pmt/builder/state_machine.hpp"
#include "pmt/rt/primitives.hpp"

namespace pmt::builder {

class StateMachineDeterminizer {
public:
 // -$ Functions $-
 // --$ Other $--
 static auto determinize(StateMachine const& input_state_machine_, pmt::rt::StateNrType state_nr_from_ = pmt::rt::StateNrStart) -> StateMachine;
};

}  // namespace pmt::builder