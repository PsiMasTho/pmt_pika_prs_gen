#include "pmt/parserbuilder/sequence_expression_to_fa_part_frame.hpp"

#include "pmt/parserbuilder/expression_to_fa_part_frame_factory.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsert;

void SequenceExpressionToFaPartFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
  }
}

void SequenceExpressionToFaPartFrame::process_stage_0(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  callstack_.push(ExpressionToFaPartFrameFactory::construct(GenericAst::AstPositionConst{&cur_expr, _idx}));
}

void SequenceExpressionToFaPartFrame::process_stage_1(CallstackType& callstack_, Captures& captures_) {
  _stage = 0;
  ++_idx;

  // If is first
  if (_idx == 1) {
    _sub_part = captures_._ret_part.take();
  } else {
    _sub_part.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _sub_part.merge_outgoing_transitions(captures_._ret_part);
  }

  // If is last
  if (_idx == _ast_position.first->get_child_at(_ast_position.second)->get_children_size()) {
    captures_._ret_part = _sub_part;
  } else {
    callstack_.push(shared_from_this());
  }
}

}  // namespace pmt::parserbuilder