// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/integer_literal_expression_to_state_machine_part_frame.hpp"
#endif
// clang-format on

#include "pmt/parserbuilder/grm_number.hpp"

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
void IntegerLiteralExpressionToFaPartFrame<TAG_>::process(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_) {
  pmt::util::parsect::State::StateNrType const state_nr_incoming = captures_._dest_state_machine.create_new_state();
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);
  captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, GrmNumber::single_char_as_value(*_path.resolve(captures_._ast)));
}

}  // namespace pmt::parserbuilder