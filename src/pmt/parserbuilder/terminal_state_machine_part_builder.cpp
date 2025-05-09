#include "pmt/parserbuilder/terminal_state_machine_part_builder.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smct;
using namespace pmt::util::smrt;

namespace {

auto is_last(size_t idx_, size_t idx_max_) -> bool {
  return idx_ == idx_max_ - 1;
}

auto is_chunk_first(size_t idx_, size_t idx_chunk_, GrmNumber::RepetitionRangeType const& range_) -> bool {
  size_t const idx_chunk_start = (idx_chunk_ == 0) ? 0 : idx_chunk_ * range_.first + ((idx_chunk_ - 1) * idx_chunk_) / 2;
  return idx_ == idx_chunk_start;
}

auto is_chunk_last(size_t idx_, size_t idx_chunk_, GrmNumber::RepetitionRangeType const& range_) -> bool {
  size_t const idx_chunk_end = (idx_chunk_ + 1) * range_.first + (idx_chunk_ * (idx_chunk_ + 1)) / 2;
  return idx_ == idx_chunk_end - 1;
}

}  // namespace

auto TerminalStateMachinePartBuilder::build(TerminalLabelLookupFn fn_lookup_terminal_name_, TerminalReverseLabelLookupFn fn_rev_lookup_terminal_name_, TerminalDefinitionLookupFn fn_lookup_terminal_definition_, pmt::util::smrt::GenericAst const& ast_root_, size_t terminal_idx_, pmt::util::smct::StateMachine& dest_state_machine_) -> StateMachinePart {
  _fn_lookup_terminal_label = std::move(fn_lookup_terminal_name_);
  _fn_rev_lookup_terminal_label = std::move(fn_rev_lookup_terminal_name_);
  _fn_lookup_terminal_definition = std::move(fn_lookup_terminal_definition_);
  _ast_root = &ast_root_;
  _terminal_idx_stack.clear();
  _terminal_idx_stack_contents.clear();

  _dest_state_machine = &dest_state_machine_;

  _terminal_idx_stack_contents.insert(Interval<size_t>(terminal_idx_));
  _terminal_idx_stack.push_back(terminal_idx_);

  _callstack.emplace_back();
  _callstack.back()._expr_cur_path = *_fn_lookup_terminal_definition(terminal_idx_);
  _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();

  while (!_callstack.empty()) {
   _keep_current_frame = false; 
   size_t const frame_idx = _callstack.size() - 1;
   dispatch(frame_idx);
   if (!_keep_current_frame) {
    _callstack.erase(_callstack.begin() + frame_idx);
   }
  }

  return std::move(_ret_part);
}

void TerminalStateMachinePartBuilder::dispatch(size_t frame_idx_) {
  switch (_callstack[frame_idx_]._expression_type) {
    case GrmAst::NtTerminalDefinition:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_definition_stage_0(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::NtTerminalSequence:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_sequence_stage_0(frame_idx_);
          break;
        case 1:
          process_sequence_stage_1(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::NtTerminalChoices:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_choices_stage_0(frame_idx_);
          break;
        case 1:
          process_choices_stage_1(frame_idx_);
          break;
        case 2:
          process_choices_stage_2(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::NtTerminalRepetition:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_repetition_stage_0(frame_idx_);
          break;
        case 1:
          process_repetition_stage_1(frame_idx_);
          break;
        case 2:
          process_repetition_stage_2(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::TkStringLiteral:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_string_literal_stage_0(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::NtTerminalRange:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_range_stage_0(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::TkIntegerLiteral:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_integer_literal_stage_0(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::TkEpsilon:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_epsilon_stage_0(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::TkTerminalIdentifier:
      switch (_callstack[frame_idx_]._stage) {
        case 0:
          process_terminal_identifier_stage_0(frame_idx_);
          break;
        case 1:
          process_terminal_identifier_stage_1(frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    default: {
      pmt::unreachable();
    }
  }
}

auto TerminalStateMachinePartBuilder::build_epsilon() -> pmt::util::smct::StateMachinePart {
  StateNrType const state_nr_incoming = _dest_state_machine->create_new_state();
  pmt::util::smct::StateMachinePart ret(state_nr_incoming);
  ret.add_outgoing_epsilon_transition(state_nr_incoming);
  return ret;
}

void TerminalStateMachinePartBuilder::process_definition_stage_0(size_t frame_idx_) {
 _callstack.emplace_back();
 _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(0);
 _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
}

void TerminalStateMachinePartBuilder::process_sequence_stage_0(size_t frame_idx_) {
  _keep_current_frame = true;
  ++_callstack[frame_idx_]._stage;

  _callstack.emplace_back();
  _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(_callstack[frame_idx_]._idx);
  _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
  
}

void TerminalStateMachinePartBuilder::process_sequence_stage_1(size_t frame_idx_) {
  _callstack[frame_idx_]._stage = 0;
  ++_callstack[frame_idx_]._idx;

  // If is first
  if (_callstack[frame_idx_]._idx == 1) {
    _callstack[frame_idx_]._sub_part = _ret_part;
    _ret_part.clear_incoming_state_nr();
    _ret_part.clear_outgoing_transitions();
  } else {
    _callstack[frame_idx_]._sub_part.connect_outgoing_transitions_to(*_ret_part.get_incoming_state_nr(), *_dest_state_machine);
    _callstack[frame_idx_]._sub_part.merge_outgoing_transitions(_ret_part);
  }

  // If is last
  if (_callstack[frame_idx_]._idx == _callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root)->get_children_size()) {
    _ret_part = _callstack[frame_idx_]._sub_part;
  } else {
    _keep_current_frame = true;
  }
}

void TerminalStateMachinePartBuilder::process_choices_stage_0(size_t frame_idx_) {
  StateNrType state_nr_incoming = _dest_state_machine->create_new_state();
  _callstack[frame_idx_]._state_cur = _dest_state_machine->get_state(state_nr_incoming);
  _callstack[frame_idx_]._sub_part.set_incoming_state_nr(state_nr_incoming);

  _keep_current_frame = true;
  ++_callstack[frame_idx_]._stage;
}

void TerminalStateMachinePartBuilder::process_choices_stage_1(size_t frame_idx_) {
  _keep_current_frame = true;
  ++_callstack[frame_idx_]._stage;

  _callstack.emplace_back();
  _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(_callstack[frame_idx_]._idx);
  _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
}

void TerminalStateMachinePartBuilder::process_choices_stage_2(size_t frame_idx_) {
  --_callstack[frame_idx_]._stage;
  ++_callstack[frame_idx_]._idx;

  _callstack[frame_idx_]._state_cur->add_epsilon_transition(*_ret_part.get_incoming_state_nr());
  _callstack[frame_idx_]._sub_part.merge_outgoing_transitions(_ret_part);

  if (_callstack[frame_idx_]._idx < _callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root)->get_children_size()) {
    _keep_current_frame = true;
    return;
  }

  _ret_part = _callstack[frame_idx_]._sub_part;
}

void TerminalStateMachinePartBuilder::process_repetition_stage_0(size_t frame_idx_) {
  _callstack[frame_idx_]._range = GrmNumber::get_repetition_range(*_callstack[frame_idx_]._expr_cur_path.clone_push(1).resolve(*_ast_root));

  if (_callstack[frame_idx_]._range.second == 0) {
    _ret_part = build_epsilon();
    return;
  }

  StateNrType const state_nr_choices = _dest_state_machine->create_new_state();
  _callstack[frame_idx_]._state_choices = _dest_state_machine->get_state(state_nr_choices);
  _callstack[frame_idx_]._choices.set_incoming_state_nr(state_nr_choices);

  if (_callstack[frame_idx_]._range.first == 0) {
    StateMachinePart tmp = build_epsilon();
    _callstack[frame_idx_]._state_choices->add_epsilon_transition(*tmp.get_incoming_state_nr());
    _callstack[frame_idx_]._choices.merge_outgoing_transitions(tmp);
    _callstack[frame_idx_]._range.first = 1;
  }

  _callstack[frame_idx_]._idx_max = ((_callstack[frame_idx_]._range.second.value_or(_callstack[frame_idx_]._range.first) - _callstack[frame_idx_]._range.first + 1) * (_callstack[frame_idx_]._range.second.value_or(_callstack[frame_idx_]._range.first) + _callstack[frame_idx_]._range.first)) / 2;

  _keep_current_frame = true;
  ++_callstack[frame_idx_]._stage;
}

void TerminalStateMachinePartBuilder::process_repetition_stage_1(size_t frame_idx_) {
  _keep_current_frame = true;
  ++_callstack[frame_idx_]._stage;

  _callstack.emplace_back();
  _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(0);
  _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
}

void TerminalStateMachinePartBuilder::process_repetition_stage_2(size_t frame_idx_) {
  --_callstack[frame_idx_]._stage;

  if (is_chunk_first(_callstack[frame_idx_]._idx, _callstack[frame_idx_]._idx_chunk, _callstack[frame_idx_]._range)) {
    _callstack[frame_idx_]._chunk = _ret_part;
    _ret_part.clear_incoming_state_nr();
    _ret_part.clear_outgoing_transitions();
  } else {
    _callstack[frame_idx_]._chunk.connect_outgoing_transitions_to(*_ret_part.get_incoming_state_nr(), *_dest_state_machine);
    _callstack[frame_idx_]._chunk.merge_outgoing_transitions(_ret_part);
  }

  if (is_chunk_last(_callstack[frame_idx_]._idx, _callstack[frame_idx_]._idx_chunk, _callstack[frame_idx_]._range)) {
    _callstack[frame_idx_]._state_choices->add_epsilon_transition(*_callstack[frame_idx_]._chunk.get_incoming_state_nr());
    _callstack[frame_idx_]._choices.merge_outgoing_transitions(_callstack[frame_idx_]._chunk);
    ++_callstack[frame_idx_]._idx_chunk;
  }

  if (!is_last(_callstack[frame_idx_]._idx, _callstack[frame_idx_]._idx_max)) {
    _keep_current_frame = true;

  } else {
    if (!_callstack[frame_idx_]._range.second.has_value()) {
      StateNrType state_nr_loop_back = _dest_state_machine->create_new_state();
      State& state_loop_back = *_dest_state_machine->get_state(state_nr_loop_back);
      state_loop_back.add_epsilon_transition(*_callstack[frame_idx_]._choices.get_incoming_state_nr());
      _callstack[frame_idx_]._choices.connect_outgoing_transitions_to(state_nr_loop_back, *_dest_state_machine);
      _callstack[frame_idx_]._choices.add_outgoing_epsilon_transition(state_nr_loop_back);
    }

    _ret_part = _callstack[frame_idx_]._choices;
  }

  ++_callstack[frame_idx_]._idx;
}

void TerminalStateMachinePartBuilder::process_string_literal_stage_0(size_t frame_idx_) {
  // Create a new incoming state
  StateNrType state_nr_prev = _dest_state_machine->create_new_state();
  pmt::util::smct::State* state_prev = _dest_state_machine->get_state(state_nr_prev);
  _ret_part.set_incoming_state_nr(state_nr_prev);

  GenericAst const& expr_cur = *_callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root);

  for (size_t i = 1; i < expr_cur.get_string().size(); ++i) {
    StateNrType state_nr_cur = _dest_state_machine->create_new_state();
    State* state_cur = _dest_state_machine->get_state(state_nr_cur);

    state_prev->add_symbol_transition(Symbol(expr_cur.get_string()[i - 1]), state_nr_cur);
    state_prev = state_cur;
    state_nr_prev = state_nr_cur;
  }

  _ret_part.add_outgoing_symbol_transition(state_nr_prev, Symbol(expr_cur.get_string().back()));
}

void TerminalStateMachinePartBuilder::process_range_stage_0(size_t frame_idx_) {
  // Create a new incoming state
  StateNrType state_nr_incoming = _dest_state_machine->create_new_state();
  _ret_part.set_incoming_state_nr(state_nr_incoming);

  GenericAst const& expr_cur = *_callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root);

  SymbolType const min = GrmNumber::single_char_as_value(*expr_cur.get_child_at(0));
  SymbolType const max = GrmNumber::single_char_as_value(*expr_cur.get_child_at(1));

  for (SymbolType i = min; i <= max; ++i) {
    _ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(i));
  }
}

void TerminalStateMachinePartBuilder::process_integer_literal_stage_0(size_t frame_idx_) {
  StateNrType const state_nr_incoming = _dest_state_machine->create_new_state();
  _ret_part.set_incoming_state_nr(state_nr_incoming);
  _ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(GrmNumber::single_char_as_value(*_callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root))));
}

void TerminalStateMachinePartBuilder::process_epsilon_stage_0(size_t) {
  _ret_part = build_epsilon();
}

void TerminalStateMachinePartBuilder::process_terminal_identifier_stage_0(size_t frame_idx_) {
  GenericAst const& expr_cur = *_callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root);

  std::string const& terminal_name = expr_cur.get_string();
  _callstack[frame_idx_]._terminal_idx_cur = _fn_rev_lookup_terminal_label(terminal_name).value_or(std::numeric_limits<size_t>::max());

  if (_callstack[frame_idx_]._terminal_idx_cur == std::numeric_limits<size_t>::max()) {
    throw std::runtime_error("Terminal '" + terminal_name + "' not defined");
  }

  _terminal_idx_stack.push_back(_callstack[frame_idx_]._terminal_idx_cur);
  if (_terminal_idx_stack_contents.contains(_callstack[frame_idx_]._terminal_idx_cur)) {
    std::string msg = "Terminal '" + terminal_name + "' is recursive: ";
    std::string delim;
    for (size_t const stack_terminal_idx : _terminal_idx_stack) {
      msg += std::exchange(delim, " -> ") + _fn_lookup_terminal_label(stack_terminal_idx).value_or("<unknown>");
    }
    throw std::runtime_error(msg);
  }

  _terminal_idx_stack_contents.insert(Interval<size_t>(_callstack[frame_idx_]._terminal_idx_cur));

  ++_callstack[frame_idx_]._stage;
  _keep_current_frame = true;

  _callstack[frame_idx_]._state_nr_reference = _dest_state_machine->create_new_state();
  _callstack[frame_idx_]._state_reference = _dest_state_machine->get_state(_callstack[frame_idx_]._state_nr_reference);

  _callstack.emplace_back();
  _callstack.back()._expr_cur_path = *_fn_lookup_terminal_definition(_callstack[frame_idx_]._terminal_idx_cur);
  _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
  
}

void TerminalStateMachinePartBuilder::process_terminal_identifier_stage_1(size_t frame_idx_) {
  _callstack[frame_idx_]._state_reference->add_epsilon_transition(*_ret_part.get_incoming_state_nr());
  _ret_part.set_incoming_state_nr(_callstack[frame_idx_]._state_nr_reference);

  _terminal_idx_stack_contents.erase(Interval<size_t>(_callstack[frame_idx_]._terminal_idx_cur));
  _terminal_idx_stack.pop_back();
}

}  // namespace pmt::parserbuilder