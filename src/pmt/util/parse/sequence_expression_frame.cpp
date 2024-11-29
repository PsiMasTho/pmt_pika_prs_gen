#include "pmt/util/parse/sequence_expression_frame.hpp"

#include "pmt/util/parse/expression_frame_factory.hpp"
#include "pmt/util/parse/grm_ast.hpp"

#include <cassert>

namespace pmt::util::parse {

SequenceExpressionFrame::SequenceExpressionFrame(AstPositionConst ast_position_)
 : _ast_position(std::move(ast_position_))
 , _stage(0)
 , _idx(0) {
}

auto SequenceExpressionFrame::get_id() const -> GenericAst::IdType {
  return GrmAst::NtSequence;
}

void SequenceExpressionFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
  }
}

void SequenceExpressionFrame::process_stage_0(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  GenericAst const& cur_expr = *_ast_position.first->get_child_at(_ast_position.second);
  callstack_.push(ExpressionFrameFactory::construct(AstPositionConst{&cur_expr, _idx}));
}

void SequenceExpressionFrame::process_stage_1(CallstackType& callstack_, Captures& captures_) {
  if (_idx == 0) {
    _sub_part = captures_._ret_part;
    ++_idx;
  }

  if (_idx > 0 && _idx < _ast_position.first->get_child_at(_ast_position.second)->get_children_size() - 1) {
    _sub_part.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _sub_part.merge_outgoing_transitions(captures_._ret_part);
    callstack_.push(shared_from_this());
    _stage = 0;
    ++_idx;
    return;
  }

  if (_idx == _ast_position.first->get_child_at(_ast_position.second)->get_children_size() - 1) {
    _sub_part.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _sub_part.merge_outgoing_transitions(captures_._ret_part);
  }

  captures_._ret_part = _sub_part;
}

}  // namespace pmt::util::parse