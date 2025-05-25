#include "pmt/parserbuilder/state_machine_part_builder.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smct;
using namespace pmt::util::smrt;

namespace {

class Frame {
 public:
  StateMachinePart _sub_part;
  GenericId::IdType _expression_type;  // Type of expression that this frame is processing
  size_t _stage = 0;                                    // Stage of processing, simulates executing other frames before returning to this one
  size_t _idx = 0;
  size_t _idx_max = 0;
  size_t _idx_chunk = 0;
  size_t _nonterminal_idx_cur;
  StateNrType _state_nr_open_cur;


  size_t _terminal_idx_cur;

  GenericAstPath _expr_cur_path;  // The expression that this frame is processing

  StateMachinePart _choices;
  StateMachinePart _chunk;
  GrmNumber::RepetitionRangeType _range;

  State* _state_cur = nullptr;
  State* _state_choices = nullptr;
};

class Locals {
 public:
  StateMachinePart _ret_part;
  pmt::base::IntervalSet<size_t> _terminal_idx_stack_contents;
  std::vector<size_t> _terminal_idx_stack;
  std::unordered_map<size_t, StateNrType> _nonterminal_idx_to_state_nr_post_open;
  std::unordered_map<size_t, StateNrType> _nonterminal_idx_to_state_nr_close;
  std::vector<Frame> _callstack;

  bool _keep_current_frame : 1 = false;
};

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

auto build_epsilon(StateMachinePartBuilder::ArgsBase const& args_) -> StateMachinePart {
  StateNrType const state_nr_incoming = args_._dest_state_machine.create_new_state();
  StateMachinePart ret(state_nr_incoming);
  ret.add_outgoing_epsilon_transition(state_nr_incoming);
  return ret;
}

void process_definition_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
 locals_._callstack.emplace_back();
 locals_._callstack.back()._expr_cur_path = locals_._callstack[frame_idx_]._expr_cur_path.clone_push(0);
 locals_._callstack.back()._expression_type = locals_._callstack.back()._expr_cur_path.resolve(args_._ast_root)->get_id();
}

void process_sequence_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  locals_._keep_current_frame = true;
  ++locals_._callstack[frame_idx_]._stage;

  locals_._callstack.emplace_back();
  locals_._callstack.back()._expr_cur_path = locals_._callstack[frame_idx_]._expr_cur_path.clone_push(locals_._callstack[frame_idx_]._idx);
  locals_._callstack.back()._expression_type = locals_._callstack.back()._expr_cur_path.resolve(args_._ast_root)->get_id();
  
}

void process_sequence_stage_1(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  locals_._callstack[frame_idx_]._stage = 0;
  ++locals_._callstack[frame_idx_]._idx;

  // If is first
  if (locals_._callstack[frame_idx_]._idx == 1) {
    locals_._callstack[frame_idx_]._sub_part = locals_._ret_part;
    locals_._ret_part.clear_incoming_state_nr();
    locals_._ret_part.clear_outgoing_transitions();
  } else {
    locals_._callstack[frame_idx_]._sub_part.connect_outgoing_transitions_to(*locals_._ret_part.get_incoming_state_nr(), args_._dest_state_machine);
    locals_._callstack[frame_idx_]._sub_part.merge_outgoing_transitions(locals_._ret_part);
  }

  // If is last
  if (locals_._callstack[frame_idx_]._idx == locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root)->get_children_size()) {
    locals_._ret_part = locals_._callstack[frame_idx_]._sub_part;
  } else {
    locals_._keep_current_frame = true;
  }
}

void process_choices_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  StateNrType state_nr_incoming = args_._dest_state_machine.create_new_state();
  locals_._callstack[frame_idx_]._state_cur = args_._dest_state_machine.get_state(state_nr_incoming);
  locals_._callstack[frame_idx_]._sub_part.set_incoming_state_nr(state_nr_incoming);

  locals_._keep_current_frame = true;
  ++locals_._callstack[frame_idx_]._stage;
}

void process_choices_stage_1(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  locals_._keep_current_frame = true;
  ++locals_._callstack[frame_idx_]._stage;

  locals_._callstack.emplace_back();
  locals_._callstack.back()._expr_cur_path = locals_._callstack[frame_idx_]._expr_cur_path.clone_push(locals_._callstack[frame_idx_]._idx);
  locals_._callstack.back()._expression_type = locals_._callstack.back()._expr_cur_path.resolve(args_._ast_root)->get_id();
}

void process_choices_stage_2(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  --locals_._callstack[frame_idx_]._stage;
  ++locals_._callstack[frame_idx_]._idx;

  locals_._callstack[frame_idx_]._state_cur->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
  locals_._callstack[frame_idx_]._sub_part.merge_outgoing_transitions(locals_._ret_part);

  if (locals_._callstack[frame_idx_]._idx < locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root)->get_children_size()) {
    locals_._keep_current_frame = true;
    return;
  }

  locals_._ret_part = locals_._callstack[frame_idx_]._sub_part;
}

void process_repetition_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  locals_._callstack[frame_idx_]._range = GrmNumber::get_repetition_range(*locals_._callstack[frame_idx_]._expr_cur_path.clone_push(1).resolve(args_._ast_root));

  if (locals_._callstack[frame_idx_]._range.second == 0) {
    locals_._ret_part = build_epsilon(args_);
    return;
  }

  StateNrType const state_nr_choices = args_._dest_state_machine.create_new_state();
  locals_._callstack[frame_idx_]._state_choices = args_._dest_state_machine.get_state(state_nr_choices);
  locals_._callstack[frame_idx_]._choices.set_incoming_state_nr(state_nr_choices);

  if (locals_._callstack[frame_idx_]._range.first == 0) {
    StateMachinePart tmp = build_epsilon(args_);
    locals_._callstack[frame_idx_]._state_choices->add_epsilon_transition(*tmp.get_incoming_state_nr());
    locals_._callstack[frame_idx_]._choices.merge_outgoing_transitions(tmp);
    locals_._callstack[frame_idx_]._range.first = 1;
  }

  locals_._callstack[frame_idx_]._idx_max = ((locals_._callstack[frame_idx_]._range.second.value_or(locals_._callstack[frame_idx_]._range.first) - locals_._callstack[frame_idx_]._range.first + 1) * (locals_._callstack[frame_idx_]._range.second.value_or(locals_._callstack[frame_idx_]._range.first) + locals_._callstack[frame_idx_]._range.first)) / 2;

  locals_._keep_current_frame = true;
  ++locals_._callstack[frame_idx_]._stage;
}

void process_repetition_stage_1(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  locals_._keep_current_frame = true;
  ++locals_._callstack[frame_idx_]._stage;

  locals_._callstack.emplace_back();
  locals_._callstack.back()._expr_cur_path = locals_._callstack[frame_idx_]._expr_cur_path.clone_push(0);
  locals_._callstack.back()._expression_type = locals_._callstack.back()._expr_cur_path.resolve(args_._ast_root)->get_id();
}

void process_repetition_stage_2(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  --locals_._callstack[frame_idx_]._stage;

  if (is_chunk_first(locals_._callstack[frame_idx_]._idx, locals_._callstack[frame_idx_]._idx_chunk, locals_._callstack[frame_idx_]._range)) {
    locals_._callstack[frame_idx_]._chunk = locals_._ret_part;
    locals_._ret_part.clear_incoming_state_nr();
    locals_._ret_part.clear_outgoing_transitions();
  } else {
    locals_._callstack[frame_idx_]._chunk.connect_outgoing_transitions_to(*locals_._ret_part.get_incoming_state_nr(), args_._dest_state_machine);
    locals_._callstack[frame_idx_]._chunk.merge_outgoing_transitions(locals_._ret_part);
  }

  if (is_chunk_last(locals_._callstack[frame_idx_]._idx, locals_._callstack[frame_idx_]._idx_chunk, locals_._callstack[frame_idx_]._range)) {
    locals_._callstack[frame_idx_]._state_choices->add_epsilon_transition(*locals_._callstack[frame_idx_]._chunk.get_incoming_state_nr());
    locals_._callstack[frame_idx_]._choices.merge_outgoing_transitions(locals_._callstack[frame_idx_]._chunk);
    ++locals_._callstack[frame_idx_]._idx_chunk;
  }

  if (!is_last(locals_._callstack[frame_idx_]._idx, locals_._callstack[frame_idx_]._idx_max)) {
    locals_._keep_current_frame = true;

  } else {
    if (!locals_._callstack[frame_idx_]._range.second.has_value()) {
      StateNrType state_nr_loop_back = args_._dest_state_machine.create_new_state();
      State& state_loop_back = *args_._dest_state_machine.get_state(state_nr_loop_back);
      state_loop_back.add_epsilon_transition(*locals_._callstack[frame_idx_]._choices.get_incoming_state_nr());
      locals_._callstack[frame_idx_]._choices.connect_outgoing_transitions_to(state_nr_loop_back, args_._dest_state_machine);
      locals_._callstack[frame_idx_]._choices.add_outgoing_epsilon_transition(state_nr_loop_back);
    }

    locals_._ret_part = locals_._callstack[frame_idx_]._choices;
  }

  ++locals_._callstack[frame_idx_]._idx;
}

void process_string_literal_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  // Create a new incoming state
  StateNrType state_nr_prev = args_._dest_state_machine.create_new_state();
  State* state_prev = args_._dest_state_machine.get_state(state_nr_prev);
  locals_._ret_part.set_incoming_state_nr(state_nr_prev);

  GenericAst const& expr_cur = *locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root);

  for (size_t i = 1; i < expr_cur.get_string().size(); ++i) {
    StateNrType state_nr_cur = args_._dest_state_machine.create_new_state();
    State* state_cur = args_._dest_state_machine.get_state(state_nr_cur);

    state_prev->add_symbol_transition(Symbol(expr_cur.get_string()[i - 1]), state_nr_cur);
    state_prev = state_cur;
    state_nr_prev = state_nr_cur;
  }

  locals_._ret_part.add_outgoing_symbol_transition(state_nr_prev, Symbol(SymbolKindCharacter, expr_cur.get_string().back()));
}

void process_range_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  // Create a new incoming state
  StateNrType state_nr_incoming = args_._dest_state_machine.create_new_state();
  locals_._ret_part.set_incoming_state_nr(state_nr_incoming);

  GenericAst const& expr_cur = *locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root);

  SymbolType const min = GrmNumber::single_char_as_value(*expr_cur.get_child_at(0));
  SymbolType const max = GrmNumber::single_char_as_value(*expr_cur.get_child_at(1));

  for (SymbolType i = min; i <= max; ++i) {
    locals_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(SymbolKindCharacter, i));
  }
}

void process_integer_literal_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t frame_idx_) {
  StateNrType const state_nr_incoming = args_._dest_state_machine.create_new_state();
  locals_._ret_part.set_incoming_state_nr(state_nr_incoming);
  locals_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(SymbolKindCharacter, GrmNumber::single_char_as_value(*locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root))));
}

void process_epsilon_stage_0(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, size_t) {
  locals_._ret_part = build_epsilon(args_);
}

void process_nonterminal_identifier_stage_0(StateMachinePartBuilder::NonterminalBuildingArgs const& args_, Locals& locals_, size_t frame_idx_){
 GenericAst const& expr_cur = *locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root);

 std::string const& nonterminal_label = expr_cur.get_string();
 locals_._callstack[frame_idx_]._nonterminal_idx_cur = args_._fn_rev_lookup_nonterminal_label(nonterminal_label);

 auto itr_close = locals_._nonterminal_idx_to_state_nr_close.find(locals_._callstack[frame_idx_]._nonterminal_idx_cur);
 if (itr_close == locals_._nonterminal_idx_to_state_nr_close.end()) {
  itr_close = locals_._nonterminal_idx_to_state_nr_close.emplace(locals_._callstack[frame_idx_]._nonterminal_idx_cur, args_._dest_state_machine.create_new_state()).first;
  State& state_close = *args_._dest_state_machine.get_state(itr_close->second);
  state_close.get_accepts().resize(locals_._callstack[frame_idx_]._nonterminal_idx_cur + 1, false);
  state_close.get_accepts().set(locals_._callstack[frame_idx_]._nonterminal_idx_cur, true);
 }

 locals_._callstack[frame_idx_]._state_nr_open_cur = args_._dest_state_machine.create_new_state();

 auto itr_post_open = locals_._nonterminal_idx_to_state_nr_post_open.find(locals_._callstack[frame_idx_]._nonterminal_idx_cur);
 if (itr_post_open == locals_._nonterminal_idx_to_state_nr_post_open.end()) {
  StateNrType const state_nr_post_open = args_._dest_state_machine.create_new_state();
  locals_._nonterminal_idx_to_state_nr_post_open.emplace(locals_._callstack[frame_idx_]._nonterminal_idx_cur, state_nr_post_open);
  ++locals_._callstack[frame_idx_]._stage;
  locals_._keep_current_frame = true;
  locals_._callstack.emplace_back();
  locals_._callstack.back()._expr_cur_path = args_._fn_lookup_definition(locals_._callstack[frame_idx_]._nonterminal_idx_cur);
  locals_._callstack.back()._expression_type = locals_._callstack.back()._expr_cur_path.resolve(args_._ast_root)->get_id();
 } else {
  locals_._ret_part.connect_outgoing_transitions_to(itr_post_open->second, args_._dest_state_machine);
  locals_._ret_part.set_incoming_state_nr(locals_._callstack[frame_idx_]._state_nr_open_cur);
  locals_._ret_part.add_outgoing_symbol_transition(locals_._callstack[frame_idx_]._state_nr_open_cur, Symbol(SymbolKindClose, locals_._callstack[frame_idx_]._nonterminal_idx_cur));
  State& state_open = *args_._dest_state_machine.get_state(locals_._callstack[frame_idx_]._state_nr_open_cur);
  state_open.add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), itr_post_open->second);
 }
}

void process_nonterminal_identifier_stage_1(StateMachinePartBuilder::NonterminalBuildingArgs const& args_, Locals& locals_, size_t frame_idx_){
 State& state_incoming = *args_._dest_state_machine.get_state(locals_._callstack[frame_idx_]._state_nr_open_cur);
 StateNrType const state_nr_close = locals_._nonterminal_idx_to_state_nr_close.find(locals_._callstack[frame_idx_]._nonterminal_idx_cur)->second;
 StateNrType const state_nr_post_open = locals_._nonterminal_idx_to_state_nr_post_open.find(locals_._callstack[frame_idx_]._nonterminal_idx_cur)->second;
 State& state_post_open = *args_._dest_state_machine.get_state(state_nr_post_open);
 state_incoming.add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), state_nr_post_open);
 state_post_open.add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 locals_._ret_part.set_incoming_state_nr(locals_._callstack[frame_idx_]._state_nr_open_cur);
 locals_._ret_part.connect_outgoing_transitions_to(state_nr_close, args_._dest_state_machine);
 locals_._ret_part.add_outgoing_symbol_transition(locals_._callstack[frame_idx_]._state_nr_open_cur, Symbol(SymbolKindClose, locals_._callstack[frame_idx_]._nonterminal_idx_cur));

 locals_._nonterminal_idx_to_state_nr_post_open.erase(locals_._callstack[frame_idx_]._nonterminal_idx_cur);
}

void process_terminal_identifier_stage_0(StateMachinePartBuilder::NonterminalBuildingArgs const& args_, Locals& locals_, size_t frame_idx_) {
 GenericAst const& expr_cur = *locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root);
 StateNrType const state_nr_incoming = args_._dest_state_machine.create_new_state();
 locals_._ret_part.set_incoming_state_nr(state_nr_incoming);
 locals_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(SymbolKindTerminal, args_._fn_rev_lookup_terminal_label(expr_cur.get_string())));
}

void process_terminal_identifier_stage_0(StateMachinePartBuilder::TerminalBuildingArgs const& args_, Locals& locals_, size_t frame_idx_) {
  GenericAst const& expr_cur = *locals_._callstack[frame_idx_]._expr_cur_path.resolve(args_._ast_root);

  std::string const& terminal_label = expr_cur.get_string();
  locals_._callstack[frame_idx_]._terminal_idx_cur = args_._fn_rev_lookup_terminal_label(terminal_label);

  locals_._terminal_idx_stack.push_back(locals_._callstack[frame_idx_]._terminal_idx_cur);
  if (locals_._terminal_idx_stack_contents.contains(locals_._callstack[frame_idx_]._terminal_idx_cur)) {
    std::string msg = "Terminal '" + terminal_label + "' is recursive: ";
    std::string delim;
    for (size_t const stack_terminal_idx : locals_._terminal_idx_stack) {
      msg += std::exchange(delim, " -> ") + args_._fn_lookup_terminal_label(stack_terminal_idx);
    }
    throw std::runtime_error(msg);
  }

  locals_._terminal_idx_stack_contents.insert(Interval<size_t>(locals_._callstack[frame_idx_]._terminal_idx_cur));

  ++locals_._callstack[frame_idx_]._stage;
  locals_._keep_current_frame = true;

  locals_._callstack.emplace_back();
  locals_._callstack.back()._expr_cur_path = args_._fn_lookup_definition(locals_._callstack[frame_idx_]._terminal_idx_cur);
  locals_._callstack.back()._expression_type = locals_._callstack.back()._expr_cur_path.resolve(args_._ast_root)->get_id();
}

void process_terminal_identifier_stage_1(Locals& locals_, size_t frame_idx_) {
  locals_._terminal_idx_stack_contents.erase(Interval<size_t>(locals_._callstack[frame_idx_]._terminal_idx_cur));
  locals_._terminal_idx_stack.pop_back();
}

void dispatch_common(auto const& args_, Locals& locals_, size_t frame_idx_) {
 switch (locals_._callstack[frame_idx_]._expression_type) {
   case GrmAst::NtNonterminalDefinition:
   case GrmAst::NtTerminalDefinition:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_definition_stage_0(args_, locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
   case GrmAst::NtNonterminalSequence:
   case GrmAst::NtTerminalSequence:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_sequence_stage_0(args_, locals_, frame_idx_);
          break;
        case 1:
          process_sequence_stage_1(args_, locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::NtNonterminalChoices:
    case GrmAst::NtTerminalChoices:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_choices_stage_0(args_, locals_, frame_idx_);
          break;
        case 1:
          process_choices_stage_1(args_, locals_, frame_idx_);
          break;
        case 2:
          process_choices_stage_2(args_, locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::NtNonterminalRepetition:
    case GrmAst::NtTerminalRepetition:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_repetition_stage_0(args_, locals_, frame_idx_);
          break;
        case 1:
          process_repetition_stage_1(args_, locals_, frame_idx_);
          break;
        case 2:
          process_repetition_stage_2(args_, locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
      case GrmAst::TkEpsilon:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_epsilon_stage_0(args_, locals_, frame_idx_);
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

void dispatch(StateMachinePartBuilder::TerminalBuildingArgs const& args_, Locals& locals_, size_t frame_idx_) {
  switch (locals_._callstack[frame_idx_]._expression_type) {
    case GrmAst::TkStringLiteral:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_string_literal_stage_0(args_, locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::NtTerminalRange:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_range_stage_0(args_, locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::TkIntegerLiteral:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_integer_literal_stage_0(args_, locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    case GrmAst::TkTerminalIdentifier:
      switch (locals_._callstack[frame_idx_]._stage) {
        case 0:
          process_terminal_identifier_stage_0(args_, locals_, frame_idx_);
          break;
        case 1:
          process_terminal_identifier_stage_1(locals_, frame_idx_);
          break;
        default:
          pmt::unreachable();
      }
      break;
    default: {
      dispatch_common(args_, locals_, frame_idx_);
    }
  }
}

void dispatch(StateMachinePartBuilder::NonterminalBuildingArgs const& args_, Locals& locals_, size_t frame_idx_) {
  switch (locals_._callstack[frame_idx_]._expression_type) {
  case GrmAst::TkNonterminalIdentifier:
    switch (locals_._callstack[frame_idx_]._stage) {
      case 0:
        process_nonterminal_identifier_stage_0(args_, locals_, frame_idx_);
        break;
      case 1:
        process_nonterminal_identifier_stage_1(args_, locals_, frame_idx_);
        break;
      default:
        pmt::unreachable();
    }
    break;
  case GrmAst::TkTerminalIdentifier:
    switch (locals_._callstack[frame_idx_]._stage) {
      case 0:
        process_terminal_identifier_stage_0(args_, locals_, frame_idx_);
        break;
      default:
        pmt::unreachable();
    }
    break;
  default: {
    dispatch_common(args_, locals_, frame_idx_);
  }
 }
}

auto build_common(auto const& args_, Locals& locals_) -> StateMachinePart {
 locals_._callstack.emplace_back();
 locals_._callstack.back()._expr_cur_path = args_._fn_lookup_definition(args_._starting_index);
 locals_._callstack.back()._expression_type = locals_._callstack.back()._expr_cur_path.resolve(args_._ast_root)->get_id();

 while (!locals_._callstack.empty()) {
  locals_._keep_current_frame = false; 
  size_t const frame_idx = locals_._callstack.size() - 1;
  dispatch(args_, locals_, frame_idx);
  if (!locals_._keep_current_frame) {
   locals_._callstack.erase(locals_._callstack.begin() + frame_idx);
  }
 }

 return std::move(locals_._ret_part);
}

}  // namespace

StateMachinePartBuilder::TerminalBuildingArgs::TerminalBuildingArgs(ArgsBase base_, LabelLookupFn fn_lookup_terminal_label_, ReverseLabelLookupFn fn_rev_lookup_terminal_label_)
: ArgsBase(std::move(base_)),
  _fn_lookup_terminal_label(std::move(fn_lookup_terminal_label_)),
  _fn_rev_lookup_terminal_label(std::move(fn_rev_lookup_terminal_label_)) {
}

StateMachinePartBuilder::NonterminalBuildingArgs::NonterminalBuildingArgs(ArgsBase base_, LabelLookupFn fn_lookup_nonterminal_label_, ReverseLabelLookupFn fn_rev_lookup_nonterminal_label_, ReverseLabelLookupFn fn_rev_lookup_terminal_label_)
: ArgsBase(std::move(base_)),
  _fn_lookup_nonterminal_label(std::move(fn_lookup_nonterminal_label_)),
  _fn_rev_lookup_nonterminal_label(std::move(fn_rev_lookup_nonterminal_label_)),
  _fn_rev_lookup_terminal_label(std::move(fn_rev_lookup_terminal_label_)) {
}

auto StateMachinePartBuilder::build(TerminalBuildingArgs const& args_) -> StateMachinePart {
 Locals locals;
 locals._terminal_idx_stack_contents.insert(Interval<size_t>(args_._starting_index));
 locals._terminal_idx_stack.push_back(args_._starting_index);

 return build_common(args_, locals);
}

auto StateMachinePartBuilder::build(NonterminalBuildingArgs const& args_) -> StateMachinePart {
 Locals locals;
 return build_common(args_, locals);
}

}  // namespace pmt::parserbuilder