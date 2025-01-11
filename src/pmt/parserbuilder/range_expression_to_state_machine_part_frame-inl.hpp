// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/range_expression_to_state_machine_part_frame.hpp"
#endif
// clang-format on

#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/parsect/state_traits.hpp"
#include "pmt/util/parsert/generic_ast.hpp"

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
void RangeExpressionToStateMachinePartFrame::process(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType&, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  // Create a new incoming state
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  GenericAst const& cur_expr = *_path.resolve(captures_._ast);

  pmt::util::parsect::StateBase::SymbolType const min = GrmNumber::single_char_as_value(*cur_expr.get_child_at(0));
  pmt::util::parsect::StateBase::SymbolType const max = GrmNumber::single_char_as_value(*cur_expr.get_child_at(1));

  for (pmt::util::parsect::StateBase::SymbolType i = min; i <= max; ++i) {
    captures_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, StateTraits<StateTagFsm>::CharacterSymbolKind, i);
  }
}

}  // namespace pmt::parserbuilder