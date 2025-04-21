#include "pmt/parserbuilder/sequence_expression_to_state_machine_part_frame.hpp"

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_factory.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smrt;

void SequenceExpressionToStateMachinePartFrame::process(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
  }
}

void SequenceExpressionToStateMachinePartFrame::process_stage_0(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  callstack_.push(shared_from_this());
  ++_stage;

  callstack_.push(ExpressionToStateMachinePartFrameFactory::construct(captures_._ast, _path.clone_push(_idx)));
}

void SequenceExpressionToStateMachinePartFrame::process_stage_1(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  _stage = 0;
  ++_idx;

  // If is first
  if (_idx == 1) {
    _sub_part = captures_._ret_part;
    captures_._ret_part.clear_incoming_state_nr();
    captures_._ret_part.clear_outgoing_transitions();
  } else {
    _sub_part.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._dest_state_machine);
    _sub_part.merge_outgoing_transitions(captures_._ret_part);
  }

  // If is last
  if (_idx == _path.resolve(captures_._ast)->get_children_size()) {
    captures_._ret_part = _sub_part;
  } else {
    callstack_.push(shared_from_this());
  }
}

}  // namespace pmt::parserbuilder