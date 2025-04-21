#include "pmt/parserbuilder/choices_expression_to_state_machine_part_frame.hpp"

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_factory.hpp"

#include "pmt/util/smrt/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smct;
using namespace pmt::util::smrt;

void ChoicesExpressionToStateMachinePartFrame::process(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(callstack_, captures_);
      break;
    case 2:
      process_stage_2(callstack_, captures_);
      break;
  }
}

void ChoicesExpressionToStateMachinePartFrame::process_stage_0(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  StateNrType state_nr_incoming = captures_._dest_state_machine.create_new_state();
  _state_cur = captures_._dest_state_machine.get_state(state_nr_incoming);
  _sub_part.set_incoming_state_nr(state_nr_incoming);

  callstack_.push(shared_from_this());
  ++_stage;
}

void ChoicesExpressionToStateMachinePartFrame::process_stage_1(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  callstack_.push(shared_from_this());
  ++_stage;

  callstack_.push(ExpressionToStateMachinePartFrameFactory::construct(captures_._ast, _path.clone_push(_idx)));
}

void ChoicesExpressionToStateMachinePartFrame::process_stage_2(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  --_stage;
  ++_idx;

  _state_cur->add_epsilon_transition(*captures_._ret_part.get_incoming_state_nr());
  _sub_part.merge_outgoing_transitions(captures_._ret_part);

  if (_idx < _path.resolve(captures_._ast)->get_children_size()) {
    callstack_.push(shared_from_this());
    return;
  }

  captures_._ret_part = _sub_part;
}

}  // namespace pmt::parserbuilder