#include "pmt/parserbuilder/range_expression_to_state_machine_part_frame.hpp"

#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/smct/symbol.hpp"
#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;
using namespace pmt::util::smct;

void RangeExpressionToStateMachinePartFrame::process(ExpressionToStateMachinePartFrameBase::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  // Create a new incoming state
  StateNrType state_nr_incoming = captures_._dest_state_machine.create_new_state();
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  GenericAst const& cur_expr = *_path.resolve(captures_._ast);

  SymbolType const min = GrmNumber::single_char_as_value(*cur_expr.get_child_at(0));
  SymbolType const max = GrmNumber::single_char_as_value(*cur_expr.get_child_at(1));

  for (SymbolType i = min; i <= max; ++i) {
    captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(i));
  }
}

}  // namespace pmt::parserbuilder