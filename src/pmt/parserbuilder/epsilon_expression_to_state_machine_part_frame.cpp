#include "pmt/parserbuilder/epsilon_expression_to_state_machine_part_frame.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;

void EpsilonExpressionToStateMachinePartFrame::process(ExpressionToStateMachinePartFrameBase::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  captures_._ret_part = make_epsilon(captures_._dest_state_machine);
}

auto EpsilonExpressionToStateMachinePartFrame::make_epsilon(pmt::util::smct::StateMachine& state_machine_) -> StateNrType {
  StateNrType const state_nr_incoming = state_machine_.create_new_state();
  pmt::util::smct::StateMachinePart ret(state_nr_incoming);
  ret.add_outgoing_epsilon_transition(state_nr_incoming);
  return state_nr_incoming;
}

}  // namespace pmt::parserbuilder