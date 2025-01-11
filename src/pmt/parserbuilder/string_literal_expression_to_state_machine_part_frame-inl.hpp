// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/string_literal_expression_to_state_machine_part_frame.hpp"
#endif
// clang-format on

#include "pmt/util/parsert/generic_ast.hpp"

namespace pmt::parserbuilder {

template <util::parsect::IsStateTag TAG_>
void StringLiteralExpressionToStateMachinePartFrame<TAG_>::process(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_) {
  // Create a new incoming state
  pmt::util::parsect::State::StateNrType state_nr_prev = captures_._result.get_unused_state_nr();
  pmt::util::parsect::State<TAG_>* state_prev = &captures_._dest_state_machine.get_state(state_nr_prev);
  captures_._ret_part.set_incoming_state_nr(state_nr_prev);

  pmt::util::parsert::GenericAst const& cur_expr = *_path.resolve(captures_._ast);

  for (size_t i = 1; i < cur_expr.get_string().size(); ++i) {
    pmt::util::parse::State::StateNrType state_nr_cur = captures_._result.get_unused_state_nr();
    Fa::State* state_cur = &captures_._result._states[state_nr_cur];

    state_prev->_transitions._symbol_transitions[cur_expr.get_string()[i - 1]] = state_nr_cur;
    state_prev = state_cur;
    state_nr_prev = state_nr_cur;
  }

  captures_._ret_part.add_outgoing_symbol_transition(state_nr_prev, cur_expr.get_string().back());
}

}  // namespace pmt::parserbuilder