#include "pmt/parserbuilder/nonterminal_state_machine_part_builder.hpp"

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
}

auto NonterminalStateMachinePartBuilder::build(NonterminalLabelLookupFn fn_lookup_nonterminal_name_, NonterminalReverseLabelLookupFn fn_rev_lookup_nonterminal_name_, NonterminalDefinitionLookupFn fn_lookup_nonterminal_definition_, TerminalIndexLookupFn fn_lookup_terminal_index_, pmt::util::smrt::GenericAst const& ast_root_, pmt::util::smrt::GenericAstPath start_nonterminal_definition_, pmt::util::smct::StateMachine& dest_state_machine_) -> pmt::util::smct::StateMachinePart{
 _fn_lookup_nonterminal_label = std::move(fn_lookup_nonterminal_name_);
 _fn_rev_lookup_nonterminal_label = std::move(fn_rev_lookup_nonterminal_name_);
 _fn_lookup_nonterminal_definition = std::move(fn_lookup_nonterminal_definition_);
 _fn_lookup_terminal_index = std::move(fn_lookup_terminal_index_);
 _ast_root = &ast_root_;

 _dest_state_machine = &dest_state_machine_;

 _callstack.emplace_back();
 _callstack.back()._expr_cur_path = std::move(start_nonterminal_definition_);
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

void NonterminalStateMachinePartBuilder::dispatch(size_t frame_idx_){
 switch (_callstack[frame_idx_]._expression_type) {
  case GrmAst::NtNonterminalDefinition:
    switch (_callstack[frame_idx_]._stage) {
      case 0:
        process_definition_stage_0(frame_idx_);
        break;
      default:
        pmt::unreachable();
    }
    break;
  case GrmAst::NtNonterminalSequence:
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
  case GrmAst::NtNonterminalChoices:
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
  case GrmAst::NtNonterminalRepetition:
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
  case GrmAst::TkNonterminalIdentifier:
    switch (_callstack[frame_idx_]._stage) {
      case 0:
        process_nonterminal_identifier_stage_0(frame_idx_);
        break;
      case 1:
        process_nonterminal_identifier_stage_1(frame_idx_);
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
    default: {
     pmt::unreachable();
    }
  }
  
}

auto NonterminalStateMachinePartBuilder::build_epsilon() -> pmt::util::smct::StateMachinePart{
 StateNrType const state_nr_incoming = _dest_state_machine->create_new_state();
 pmt::util::smct::StateMachinePart ret(state_nr_incoming);
 ret.add_outgoing_epsilon_transition(state_nr_incoming);
 return ret;
}

void NonterminalStateMachinePartBuilder::process_definition_stage_0(size_t frame_idx_){
 _callstack.emplace_back();
 _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(0);
 _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
}

void NonterminalStateMachinePartBuilder::process_sequence_stage_0(size_t frame_idx_){
 _keep_current_frame = true;
 ++_callstack[frame_idx_]._stage;

 _callstack.emplace_back();
 _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(_callstack[frame_idx_]._idx);
 _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
}

void NonterminalStateMachinePartBuilder::process_sequence_stage_1(size_t frame_idx_){
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

void NonterminalStateMachinePartBuilder::process_choices_stage_0(size_t frame_idx_){
 StateNrType state_nr_incoming = _dest_state_machine->create_new_state();
 _callstack[frame_idx_]._state_cur = _dest_state_machine->get_state(state_nr_incoming);
 _callstack[frame_idx_]._sub_part.set_incoming_state_nr(state_nr_incoming);

 _keep_current_frame = true;
 ++_callstack[frame_idx_]._stage;
}

void NonterminalStateMachinePartBuilder::process_choices_stage_1(size_t frame_idx_){
 _keep_current_frame = true;
 ++_callstack[frame_idx_]._stage;

 _callstack.emplace_back();
 _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(_callstack[frame_idx_]._idx);
 _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
}

void NonterminalStateMachinePartBuilder::process_choices_stage_2(size_t frame_idx_){
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

void NonterminalStateMachinePartBuilder::process_repetition_stage_0(size_t frame_idx_) {
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

void NonterminalStateMachinePartBuilder::process_repetition_stage_1(size_t frame_idx_) {
 _keep_current_frame = true;
 ++_callstack[frame_idx_]._stage;

 _callstack.emplace_back();
 _callstack.back()._expr_cur_path = _callstack[frame_idx_]._expr_cur_path.clone_push(0);
 _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
}

void NonterminalStateMachinePartBuilder::process_repetition_stage_2(size_t frame_idx_) {
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

void NonterminalStateMachinePartBuilder::process_nonterminal_identifier_stage_0(size_t frame_idx_){
 GenericAst const& expr_cur = *_callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root);

 std::string const& nonterminal_label = expr_cur.get_string();
 _callstack[frame_idx_]._nonterminal_idx_cur = _fn_rev_lookup_nonterminal_label(nonterminal_label);

 auto itr_close = _nonterminal_idx_to_state_nr_close.find(_callstack[frame_idx_]._nonterminal_idx_cur);
 if (itr_close == _nonterminal_idx_to_state_nr_close.end()) {
  itr_close = _nonterminal_idx_to_state_nr_close.emplace(_callstack[frame_idx_]._nonterminal_idx_cur, _dest_state_machine->create_new_state()).first;
  State& state_close = *_dest_state_machine->get_state(itr_close->second);
  state_close.get_accepts().resize(_callstack[frame_idx_]._nonterminal_idx_cur + 1, false);
  state_close.get_accepts().set(_callstack[frame_idx_]._nonterminal_idx_cur, true);
 }

 _callstack[frame_idx_]._state_nr_open_cur = _dest_state_machine->create_new_state();

 auto itr_post_open = _nonterminal_idx_to_state_nr_post_open.find(_callstack[frame_idx_]._nonterminal_idx_cur);
 if (itr_post_open == _nonterminal_idx_to_state_nr_post_open.end()) {
  StateNrType const state_nr_post_open = _dest_state_machine->create_new_state();
  _nonterminal_idx_to_state_nr_post_open.emplace(_callstack[frame_idx_]._nonterminal_idx_cur, state_nr_post_open);
  ++_callstack[frame_idx_]._stage;
  _keep_current_frame = true;
  _callstack.emplace_back();
  _callstack.back()._expr_cur_path = _fn_lookup_nonterminal_definition(_callstack[frame_idx_]._nonterminal_idx_cur);
  _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();
 } else {
  _ret_part.connect_outgoing_transitions_to(itr_post_open->second, *_dest_state_machine);
  _ret_part.set_incoming_state_nr(_callstack[frame_idx_]._state_nr_open_cur);
  _ret_part.add_outgoing_symbol_transition(_callstack[frame_idx_]._state_nr_open_cur, Symbol(SymbolKindClose, _callstack[frame_idx_]._nonterminal_idx_cur));
  State& state_open = *_dest_state_machine->get_state(_callstack[frame_idx_]._state_nr_open_cur);
  state_open.add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), itr_post_open->second);
 }
}

void NonterminalStateMachinePartBuilder::process_nonterminal_identifier_stage_1(size_t frame_idx_){
 State& state_incoming = *_dest_state_machine->get_state(_callstack[frame_idx_]._state_nr_open_cur);
 StateNrType const state_nr_close = _nonterminal_idx_to_state_nr_close.find(_callstack[frame_idx_]._nonterminal_idx_cur)->second;
 StateNrType const state_nr_post_open = _nonterminal_idx_to_state_nr_post_open.find(_callstack[frame_idx_]._nonterminal_idx_cur)->second;
 State& state_post_open = *_dest_state_machine->get_state(state_nr_post_open);
 state_incoming.add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), state_nr_post_open);
 state_post_open.add_epsilon_transition(*_ret_part.get_incoming_state_nr());
 _ret_part.set_incoming_state_nr(_callstack[frame_idx_]._state_nr_open_cur);
 _ret_part.connect_outgoing_transitions_to(state_nr_close, *_dest_state_machine);
 _ret_part.add_outgoing_symbol_transition(_callstack[frame_idx_]._state_nr_open_cur, Symbol(SymbolKindClose, _callstack[frame_idx_]._nonterminal_idx_cur));

 _nonterminal_idx_to_state_nr_post_open.erase(_callstack[frame_idx_]._nonterminal_idx_cur);
}

void NonterminalStateMachinePartBuilder::process_terminal_identifier_stage_0(size_t frame_idx_) {
 GenericAst const& expr_cur = *_callstack[frame_idx_]._expr_cur_path.resolve(*_ast_root);
 StateNrType const state_nr_incoming = _dest_state_machine->create_new_state();
 _ret_part.set_incoming_state_nr(state_nr_incoming);
 _ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(SymbolKindTerminal, _fn_lookup_terminal_index(expr_cur.get_string())));
}

void NonterminalStateMachinePartBuilder::process_epsilon_stage_0(size_t) {
 _ret_part = build_epsilon();
}

}