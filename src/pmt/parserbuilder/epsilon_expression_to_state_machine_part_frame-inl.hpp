// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/epsilon_expression_to_state_machine_part_frame.hpp"
#endif
// clang-format on

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
void EpsilonExpressionToFaPartFrame<TAG_>::process(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_) {
  captures_._ret_part = make_epsilon(captures_._result);
}

template <pmt::util::parsect::IsStateTag TAG_>
auto EpsilonExpressionToFaPartFrame<TAG_>::make_epsilon(pmt::util::parsect::StateMachine& state_machine_) -> pmt::util::parsect::State::StateNrType {
  pmt::util::parsect::State::StateNrType const state_nr_incoming = state_machine_.get_unused_state_nr();
  state_machine_.create_new_state(state_nr_incoming);
  pmt::util::parsect::StateMachinePart<TAG_> ret(state_nr_incoming);
  ret.add_outgoing_epsilon_transition(state_nr_incoming);
  return ret;
}

}  // namespace pmt::parserbuilder