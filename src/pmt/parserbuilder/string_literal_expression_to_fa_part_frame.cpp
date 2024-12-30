#include "pmt/parserbuilder/string_literal_expression_to_fa_part_frame.hpp"

#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/util/parsect/fa.hpp"
#include "pmt/util/parsert/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

void StringLiteralExpressionToFaPartFrame::process(CallstackType&, Captures& captures_) {
  // Create a new incoming state
  Fa::StateNrType state_nr_prev = captures_._result.get_unused_state_nr();
  Fa::State* state_prev = &captures_._result._states[state_nr_prev];
  captures_._ret_part.set_incoming_state_nr(state_nr_prev);

  GenericAst const& cur_expr = *_path.resolve(captures_._ast);

  for (size_t i = 1; i < cur_expr.get_string().size(); ++i) {
    Fa::StateNrType state_nr_cur = captures_._result.get_unused_state_nr();
    Fa::State* state_cur = &captures_._result._states[state_nr_cur];

    state_prev->_transitions._symbol_transitions[cur_expr.get_string()[i - 1]] = state_nr_cur;
    state_prev = state_cur;
    state_nr_prev = state_nr_cur;
  }

  captures_._ret_part.add_outgoing_symbol_transition(state_nr_prev, cur_expr.get_string().back());
}

}  // namespace pmt::parserbuilder