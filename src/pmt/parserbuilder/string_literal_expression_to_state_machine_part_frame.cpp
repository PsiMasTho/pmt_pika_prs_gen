#include "pmt/parserbuilder/string_literal_expression_to_state_machine_part_frame.hpp"

#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;
using namespace pmt::util::smct;

void StringLiteralExpressionToStateMachinePartFrame::process(ExpressionToStateMachinePartFrameBase::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  // Create a new incoming state
  StateNrType state_nr_prev = captures_._dest_state_machine.create_new_state();
  pmt::util::smct::State* state_prev = captures_._dest_state_machine.get_state(state_nr_prev);
  captures_._ret_part.set_incoming_state_nr(state_nr_prev);

  pmt::util::smrt::GenericAst const& cur_expr = *_path.resolve(captures_._ast);

  for (size_t i = 1; i < cur_expr.get_string().size(); ++i) {
    StateNrType state_nr_cur = captures_._dest_state_machine.create_new_state();
    State* state_cur = captures_._dest_state_machine.get_state(state_nr_cur);

    state_prev->add_symbol_transition(Symbol(cur_expr.get_string()[i - 1]), state_nr_cur);
    state_prev = state_cur;
    state_nr_prev = state_nr_cur;
  }

  captures_._ret_part.add_outgoing_symbol_transition(state_nr_prev, Symbol(cur_expr.get_string().back()));
}

}  // namespace pmt::parserbuilder