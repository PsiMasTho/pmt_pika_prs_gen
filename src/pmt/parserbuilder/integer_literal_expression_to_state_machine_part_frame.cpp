#include "pmt/parserbuilder/integer_literal_expression_to_state_machine_part_frame.hpp"

#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/smct/symbol.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;
using namespace pmt::util::smct;

void IntegerLiteralExpressionToStateMachinePartFrame::process(ExpressionToStateMachinePartFrameBase::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  StateNrType const state_nr_incoming = captures_._dest_state_machine.create_new_state();
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);
  captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(GrmNumber::single_char_as_value(*_path.resolve(captures_._ast))));
}

}  // namespace pmt::parserbuilder