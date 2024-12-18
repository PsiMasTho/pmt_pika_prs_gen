#include "pmt/parserbuilder/integer_literal_expression_to_fa_part_frame.hpp"

#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/parsect/fa.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

void IntegerLiteralExpressionToFaPartFrame::process(CallstackType&, Captures& captures_) {
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);
  captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, GrmNumber::single_char_as_value(*_ast_position.first->get_child_at(_ast_position.second)));
}

}  // namespace pmt::parserbuilder