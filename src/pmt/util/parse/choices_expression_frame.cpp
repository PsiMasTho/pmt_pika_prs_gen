#include "pmt/util/parse/choices_expression_frame.hpp"

#include "pmt/util/parse/expression_frame_factory.hpp"
#include "pmt/util/parse/grm_ast.hpp"

#include <cassert>

namespace pmt::util::parse {

ChoicesExpressionFrame::ChoicesExpressionFrame(AstPositionConst ast_position_)
 : _ast_position(std::move(ast_position_))
 , _stage(0) {
}

auto ChoicesExpressionFrame::get_id() const -> GenericAst::IdType {
  return GrmAst::NtChoices;
}

void ChoicesExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
  }
}

void ChoicesExpressionFrame::process_stage_0(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  callstack_.push(ExpressionFrameFactory::construct(AstPositionConst{&cur_expr, _sub_parts.size()}));
}

void ChoicesExpressionFrame::process_stage_1(CallstackType& callstack_, Captures& captures_) {
  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);

  if (_sub_parts.size() < cur_expr.get_children_size() - 1) {
    _sub_parts.push_back(captures_._ret_part);
    callstack_.push(shared_from_this());
    _stage = 0;
    return;
  }

  _sub_parts.push_back(captures_._ret_part);

  // Create a new incoming state
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  Fa::State& state_incoming = captures_._result._states[state_nr_incoming];
  captures_._ret_part.set_incoming_state_nr(state_nr_incoming);

  // Connect with epsilon transitions to the sub-parts
  for (FaPart& part : _sub_parts) {
    state_incoming._transitions._epsilon_transitions.insert(*part.get_incoming_state_nr());
    captures_._ret_part.merge_outgoing_transitions(part);
  }
}

}  // namespace pmt::util::parse