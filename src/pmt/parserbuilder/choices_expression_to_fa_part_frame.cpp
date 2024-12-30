#include "pmt/parserbuilder/choices_expression_to_fa_part_frame.hpp"

#include "pmt/parserbuilder/expression_to_fa_part_frame_factory.hpp"
#include "pmt/util/parsert/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

void ChoicesExpressionToFaPartFrame::process(CallstackType& callstack_, Captures& captures_) {
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

void ChoicesExpressionToFaPartFrame::process_stage_0(CallstackType& callstack_, Captures& captures_) {
  Fa::StateNrType state_nr_incoming = captures_._result.get_unused_state_nr();
  _transitions = &captures_._result._states[state_nr_incoming]._transitions;
  _sub_part.set_incoming_state_nr(state_nr_incoming);

  callstack_.push(shared_from_this());
  ++_stage;
}

void ChoicesExpressionToFaPartFrame::process_stage_1(CallstackType& callstack_, Captures& captures_) {
  callstack_.push(shared_from_this());
  ++_stage;

  callstack_.push(ExpressionToFaPartFrameFactory::construct(captures_._ast, _path.clone_push(_idx)));
}

void ChoicesExpressionToFaPartFrame::process_stage_2(CallstackType& callstack_, Captures& captures_) {
  --_stage;
  ++_idx;

  _transitions->_epsilon_transitions.insert(*captures_._ret_part.get_incoming_state_nr());
  _sub_part.merge_outgoing_transitions(captures_._ret_part);

  if (_idx < _path.resolve(captures_._ast)->get_children_size()) {
    callstack_.push(shared_from_this());
    return;
  }

  captures_._ret_part = _sub_part;
}

}  // namespace pmt::parserbuilder