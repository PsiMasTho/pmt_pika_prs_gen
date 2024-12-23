#include "pmt/parserbuilder/repetition_expression_to_fa_part_frame.hpp"

#include "pmt/parserbuilder/epsilon_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/expression_to_fa_part_frame_factory.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

RepetitionExpressionToFaPartFrame::RepetitionExpressionToFaPartFrame(GenericAst::AstPositionConst ast_position_)
 : ExpressionToFaPartFrameBase({ast_position_.first->get_child_at(ast_position_.second), 0})
 , _range(GrmNumber::get_repetition_range(*ast_position_.first->get_child_at(ast_position_.second)->get_child_at(1))) {
}

void RepetitionExpressionToFaPartFrame::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(callstack_);
      break;
    case 2:
      process_stage_2(callstack_, captures_);
      break;
  }
}

void RepetitionExpressionToFaPartFrame::process_stage_0(CallstackType& callstack_, Captures& captures_) {
  if (_range.second == 0) {
    captures_._ret_part = EpsilonExpressionToFaPartFrame::make_epsilon(captures_._result);
    return;
  }

  Fa::StateNrType const state_nr_choices = captures_._result.get_unused_state_nr();
  _transitions_choices = &captures_._result._states[state_nr_choices]._transitions;
  _choices.set_incoming_state_nr(state_nr_choices);

  if (_range.first == 0) {
    FaPart tmp = EpsilonExpressionToFaPartFrame::make_epsilon(captures_._result);
    _transitions_choices->_epsilon_transitions.insert(*tmp.get_incoming_state_nr());
    _choices.merge_outgoing_transitions(tmp);
    _range.first = 1;
  }

  _idx_max = ((_range.second.value_or(_range.first) - _range.first + 1) * (_range.second.value_or(_range.first) + _range.first)) / 2;

  callstack_.push(shared_from_this());
  ++_stage;
}

void RepetitionExpressionToFaPartFrame::process_stage_1(CallstackType& callstack_) {
  callstack_.push(shared_from_this());
  ++_stage;

  callstack_.push(ExpressionToFaPartFrameFactory::construct(_ast_position));
}

void RepetitionExpressionToFaPartFrame::process_stage_2(CallstackType& callstack_, Captures& captures_) {
  --_stage;

  if (is_chunk_first()) {
    _chunk = captures_._ret_part.take();
  } else {
    _chunk.connect_outgoing_transitions_to(*captures_._ret_part.get_incoming_state_nr(), captures_._result);
    _chunk.merge_outgoing_transitions(captures_._ret_part);
  }

  if (is_chunk_last()) {
    _transitions_choices->_epsilon_transitions.insert(*_chunk.get_incoming_state_nr());
    _choices.merge_outgoing_transitions(_chunk);
    ++_idx_chunk;
  }

  if (!is_last()) {
    callstack_.push(shared_from_this());
  } else {
    if (!_range.second.has_value()) {
      Fa::StateNrType state_nr_loop_back = captures_._result.get_unused_state_nr();
      Fa::State& state_loop_back = captures_._result._states[state_nr_loop_back];
      state_loop_back._transitions._epsilon_transitions.insert(*_choices.get_incoming_state_nr());
      _choices.connect_outgoing_transitions_to(state_nr_loop_back, captures_._result);
      _choices.add_outgoing_epsilon_transition(state_nr_loop_back);
    }

    captures_._ret_part = _choices;
  }

  ++_idx;
}

auto RepetitionExpressionToFaPartFrame::is_last() const -> bool {
  return _idx == _idx_max - 1;
}

auto RepetitionExpressionToFaPartFrame::is_chunk_first() const -> bool {
  size_t const idx_chunk_start = (_idx_chunk == 0) ? 0 : _idx_chunk * _range.first + ((_idx_chunk - 1) * _idx_chunk) / 2;
  return _idx == idx_chunk_start;
}

auto RepetitionExpressionToFaPartFrame::is_chunk_last() const -> bool {
  size_t const idx_chunk_end = (_idx_chunk + 1) * _range.first + (_idx_chunk * (_idx_chunk + 1)) / 2;
  return _idx == idx_chunk_end - 1;
}

}  // namespace pmt::parserbuilder