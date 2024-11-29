#include "pmt/util/parse/string_literal_expression_frame.hpp"

#include "pmt/util/parse/grm_ast.hpp"

#include <cassert>

namespace pmt::util::parse {

StringLiteralExpressionFrame::StringLiteralExpressionFrame(AstPositionConst ast_position_)
 : _ast_position(std::move(ast_position_)) {
}

auto StringLiteralExpressionFrame::get_id() const -> GenericAst::IdType {
  return GrmAst::TkStringLiteral;
}

void StringLiteralExpressionFrame::process(CallstackType&, Captures& captures_) {
  // Create a new incoming state
  Fa::StateNrType state_nr_prev = captures_._result.get_unused_state_nr();
  Fa::State* state_prev = &captures_._result._states[state_nr_prev];
  captures_._ret_part.set_incoming_state_nr(state_nr_prev);

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  for (size_t i = 1; i < cur_expr.get_token().size(); ++i) {
    Fa::StateNrType state_nr_cur = captures_._result.get_unused_state_nr();
    Fa::State* state_cur = &captures_._result._states[state_nr_cur];

    state_prev->_transitions._symbol_transitions[cur_expr.get_token()[i - 1]] = state_nr_cur;
    state_prev = state_cur;
    state_nr_prev = state_nr_cur;
  }

  captures_._ret_part.add_outgoing_symbol_transition(state_nr_prev, cur_expr.get_token().back());
}

}  // namespace pmt::util::parse