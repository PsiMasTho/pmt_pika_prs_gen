#include "pmt/parserbuilder/epsilon_expression_to_state_machine_part_frame.hpp"

#include "pmt/util/smct/state_machine_part.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;
using namespace pmt::util::smct;

void EpsilonExpressionToStateMachinePartFrame::process(ExpressionToStateMachinePartFrameBase::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  captures_._ret_part = make_epsilon(captures_._dest_state_machine);
}

auto EpsilonExpressionToStateMachinePartFrame::make_epsilon(pmt::util::smct::StateMachine& state_machine_) -> StateMachinePart {
  StateNrType const state_nr_incoming = state_machine_.create_new_state();
  pmt::util::smct::StateMachinePart ret(state_nr_incoming);
  ret.add_outgoing_epsilon_transition(state_nr_incoming);
  return ret;
}

}  // namespace pmt::parserbuilder