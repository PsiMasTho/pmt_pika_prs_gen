#include "pmt/util/parse/range_expression_frame.hpp"

#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/grm_ast_transformations.hpp"

#include <cassert>

namespace pmt::util::parse {

RangeExpressionFrame::RangeExpressionFrame(AstPositionConst ast_position_)
 : _ast_position(std::move(ast_position_)) {
}

auto RangeExpressionFrame::get_id() const -> GenericAst::IdType {
  return GrmAst::NtRange;
}

void RangeExpressionFrame::process(CallstackType&, Captures& captures_) {
  // Create a new incoming state
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  Fa::State& state_incoming = captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  size_t const min = GrmAstTransformations::single_char_as_value(*cur_expr.get_child_at(0));
  size_t const max = GrmAstTransformations::single_char_as_value(*cur_expr.get_child_at(1));

  for (size_t i = min; i <= max; ++i) {
    Fa::StateNrType state_nr_next = captures_._result.get_unused_state_nr();
    captures_._result._states[state_nr_next];
    captures_._ret_part.add_outgoing_symbol_transition(state_nr_next, i);
    state_incoming._transitions._epsilon_transitions.insert(state_nr_next);
  }
}

}  // namespace pmt::util::parse