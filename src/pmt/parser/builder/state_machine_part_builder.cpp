#include "pmt/parser/builder/state_machine_part_builder.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/charset.hpp"
#include "pmt/parser/grammar/index_permutation_generator.hpp"
#include "pmt/parser/grammar/permute.hpp"
#include "pmt/parser/grammar/repetition_range.hpp"
#include "pmt/parser/grammar/string_literal.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/fsm_intersector.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <deque>
#include <optional>
#include <variant>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;
using namespace pmt::parser::grammar;

namespace {

class FrameBase {
public:
 GenericAstPath _expr_cur_path;
 StateMachine* _state_machine_target = nullptr;
 size_t _stage = 0;
 bool _is_hiding : 1 = false;
};

class DefinitionFrame : public FrameBase {
public:
};

class ExpressionFrame : public FrameBase {
public:
};

class SequenceFrame : public FrameBase {
public:
 StateMachinePart _sub_part;
 std::optional<GenericAstPath> _delim_cur_path;
 std::vector<size_t> _index_permutation;
 size_t _idx = 0;
 bool _is_permuted : 1 = false;
 bool _is_delimiting : 1 = false;
};

class PermuteFrame : public FrameBase {
public:
 StateMachinePart _sub_part;
 IndexPermutationGenerator _index_permutation_generator;
 State* _state_cur = nullptr;
 size_t _idx = 0;
};

class PermuteDelimitedFrame : public FrameBase {
public:
 StateMachinePart _sub_part;
 IndexPermutationGenerator _index_permutation_generator;
 State* _state_cur = nullptr;
 size_t _idx = 0;
};

class IntersectFrame : public FrameBase {
public:
 StateMachinePart _sub_part;
 StateMachine _sm_lhs;
 StateMachine _sm_rhs;
 size_t _idx = 0;
};

class ChoicesFrame : public FrameBase {
public:
 StateMachinePart _sub_part;
 State* _state_cur = nullptr;
 size_t _idx = 0;
};

class RepetitionFrame : public FrameBase {
public:
 StateMachinePart _choices;
 StateMachinePart _chunk;
 RepetitionRange _range;
 State* _state_choices = nullptr;
 size_t _idx_chunk = 0;
 size_t _idx_max = 0;
 size_t _idx = 0;
};

class StringLiteralFrame : public FrameBase {
public:
};

class IntegerLiteralFrame : public FrameBase {
public:
};

class CharsetFrame : public FrameBase {
public:
};

class EpsilonFrame : public FrameBase {
public:
};

class NonterminalIdentifierFrame : public FrameBase {
public:
 size_t _nonterminal_idx_cur = 0;
 StateNrType _state_nr_open_cur = 0;
};

class TerminalIdentifierFrame : public FrameBase {
public:
 size_t _terminal_idx_cur = 0;
};

class HiddenFrame : public FrameBase {
public:
};

using Frame = std::variant<DefinitionFrame, ExpressionFrame, SequenceFrame, PermuteFrame, PermuteDelimitedFrame, IntersectFrame, ChoicesFrame, RepetitionFrame, StringLiteralFrame, IntegerLiteralFrame, CharsetFrame, EpsilonFrame, NonterminalIdentifierFrame, TerminalIdentifierFrame, HiddenFrame>;

class Locals {
public:
 StateMachinePart _ret_part;
 std::deque<Frame> _callstack;
 std::unordered_map<size_t, StateNrType> _nonterminal_idx_to_state_nr_post_open;
 std::unordered_map<size_t, StateNrType> _nonterminal_idx_to_state_nr_close;
 std::vector<size_t> _terminal_idx_stack;
 IntervalSet<size_t> _terminal_idx_stack_contents;
 bool _keep_current_frame : 1 = false;
};

// any outgoing transitions go to an accepting state
void state_machine_part_to_state_machine(StateMachinePart const& part_, StateMachine& sm_) {
 StateNrType const state_nr_accept = sm_.create_new_state();
 sm_.get_state(state_nr_accept)->get_accepts().insert(Interval<AcceptsIndexType>(0));

 for (auto const& [state_nr_from, symbols] : part_.get_outgoing_symbol_transitions()) {
  State& state_from = *sm_.get_state(state_nr_from);
  for (auto const& [kind, intervals] : symbols) {
   intervals.for_each_interval([&](Interval<SymbolValueType> const& interval_) { state_from.add_symbol_transition(kind, interval_, state_nr_accept); });
  }
 }

 for (StateNrType const state_nr_to : part_.get_outgoing_epsilon_transitions()) {
  sm_.get_state(state_nr_to)->add_epsilon_transition(state_nr_accept);
 }
}

// merges the source state machine into the destination state machine with all
// accepting states of sm_src_ becoming outgoing transitions of the returned part
auto state_machine_merge(StateMachine& sm_dest_, StateMachine const& sm_src_) -> StateMachinePart {
 StateMachinePart ret;

 std::unordered_map<StateNrType, StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const take = [&]() {
  StateNrType const ret = pending.back();
  pending.pop_back();
  return ret;
 };

 auto const push_and_visit = [&](StateNrType const state_nr_) -> StateNrType {
  auto itr = visited.find(state_nr_);
  if (itr != visited.end()) {
   return itr->second;
  }
  itr = visited.emplace(state_nr_, sm_dest_.create_new_state()).first;
  pending.push_back(state_nr_);
  return itr->second;
 };

 ret.set_incoming_state_nr(push_and_visit(StateNrStart));

 while (!pending.empty()) {
  StateNrType const state_nr_old_cur = take();
  StateNrType const state_nr_new_cur = visited.find(state_nr_old_cur)->second;

  State const& state_old_cur = *sm_src_.get_state(state_nr_old_cur);

  State& state_new_cur = *sm_dest_.get_state(state_nr_new_cur);

  // Accepting states become outgoing transitions
  if (state_old_cur.get_accepts().popcnt() != 0) {
   ret.add_outgoing_epsilon_transition(state_nr_new_cur);
  }

  // Follow epsilon transitions
  state_old_cur.get_epsilon_transitions().for_each_key([&](StateNrType const state_nr_old_next_) { state_new_cur.add_epsilon_transition(push_and_visit(state_nr_old_next_)); });

  // Follow SymbolKindCharacter transitions
  state_old_cur.get_symbol_transitions(SymbolKindCharacter).for_each_interval([&](StateNrType const state_nr_old_next_, Interval<SymbolValueType> const& interval_) { state_new_cur.add_symbol_transition(SymbolKindCharacter, interval_, push_and_visit(state_nr_old_next_)); });

  // Follow SymbolKindHiddenCharacter transitions
  state_old_cur.get_symbol_transitions(SymbolKindHiddenCharacter).for_each_interval([&](StateNrType const state_nr_old_next_, Interval<SymbolValueType> const& interval_) { state_new_cur.add_symbol_transition(SymbolKindHiddenCharacter, interval_, push_and_visit(state_nr_old_next_)); });
 }

 return ret;
}

auto is_last(size_t idx_, size_t idx_max_) -> bool {
 return idx_ == idx_max_ - 1;
}

auto is_chunk_first(size_t idx_, size_t idx_chunk_, RepetitionRange const& range_) -> bool {
 size_t const idx_chunk_start = (idx_chunk_ == 0) ? 0 : idx_chunk_ * range_.get_lower() + ((idx_chunk_ - 1) * idx_chunk_) / 2;
 return idx_ == idx_chunk_start;
}

auto is_chunk_last(size_t idx_, size_t idx_chunk_, RepetitionRange const& range_) -> bool {
 size_t const idx_chunk_end = (idx_chunk_ + 1) * range_.get_lower() + (idx_chunk_ * (idx_chunk_ + 1)) / 2;
 return idx_ == idx_chunk_end - 1;
}

auto build_epsilon(StateMachinePartBuilder::ArgsBase const& args_, StateMachine& state_machine_target_) -> StateMachinePart {
 StateNrType const state_nr_incoming = state_machine_target_.create_new_state();
 StateMachinePart ret(state_nr_incoming);
 ret.add_outgoing_epsilon_transition(state_nr_incoming);
 return ret;
}

auto construct_frame(StateMachinePartBuilder::ArgsBase const& args_, GenericAstPath expr_cur_path_, StateMachine& state_machine_target_, bool is_hiding_) -> Frame {
 FrameBase frame_base{._expr_cur_path = std::move(expr_cur_path_), ._state_machine_target = &state_machine_target_, ._stage = 0, ._is_hiding = is_hiding_};

 switch (frame_base._expr_cur_path.resolve(args_._ast_root)->get_id()) {
  case Ast::NtNonterminalDefinition:
  case Ast::NtTerminalDefinition:
   return DefinitionFrame{std::move(frame_base)};
  case Ast::NtNonterminalExpression:
  case Ast::NtTerminalExpression:
   return ExpressionFrame{std::move(frame_base)};
  case Ast::NtTerminalSequence:
  case Ast::NtNonterminalSequence:
   return SequenceFrame{std::move(frame_base)};
  case Ast::NtPermute:
   return PermuteFrame{std::move(frame_base)};
  case Ast::NtPermuteDelimited:
   return PermuteDelimitedFrame{std::move(frame_base)};
  case Ast::NtIntersect:
   return IntersectFrame{std::move(frame_base)};
  case Ast::NtNonterminalChoices:
  case Ast::NtTerminalChoices:
   return ChoicesFrame{std::move(frame_base)};
  case Ast::NtRepetitionExpression:
   return RepetitionFrame{std::move(frame_base)};
  case Ast::TkStringLiteral:
   return StringLiteralFrame{std::move(frame_base)};
  case Ast::TkIntegerLiteral:
   return IntegerLiteralFrame{std::move(frame_base)};
  case Ast::NtTerminalCharset:
   return CharsetFrame{std::move(frame_base)};
  case Ast::TkEpsilon:
   return EpsilonFrame{std::move(frame_base)};
  case Ast::TkNonterminalIdentifier:
   return NonterminalIdentifierFrame{std::move(frame_base)};
  case Ast::TkTerminalIdentifier:
   return TerminalIdentifierFrame{std::move(frame_base)};
  case Ast::NtHidden:
   return HiddenFrame{std::move(frame_base)};
  default:
   pmt::unreachable();
 }
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, DefinitionFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(0), *frame_._state_machine_target, false));
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, ExpressionFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(0), *frame_._state_machine_target, frame_._is_hiding));
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, SequenceFrame& frame_) {
 ++frame_._stage;

 if (!frame_._is_permuted) {
  std::generate_n(std::back_inserter(frame_._index_permutation), frame_._expr_cur_path.resolve(args_._ast_root)->get_children_size(), [n = 0]() mutable { return n++; });
  frame_._is_permuted = true;
 }

 if (frame_._index_permutation.empty()) {
  locals_._ret_part = build_epsilon(args_, *frame_._state_machine_target);
  return;
 }

 locals_._keep_current_frame = true;
}

void process_frame_01(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, SequenceFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 if (frame_._is_delimiting) {
  locals_._callstack.emplace_back(construct_frame(args_, *frame_._delim_cur_path, *frame_._state_machine_target, frame_._is_hiding));
 } else {
  locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(frame_._index_permutation[frame_._idx]), *frame_._state_machine_target, frame_._is_hiding));
 }
}

void process_frame_02(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, SequenceFrame& frame_) {
 --frame_._stage;

 if (frame_._is_delimiting) {
  frame_._is_delimiting = false;
  frame_._sub_part.connect_outgoing_transitions_to(*locals_._ret_part.get_incoming_state_nr(), *frame_._state_machine_target);
  frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);
  locals_._keep_current_frame = true;
  return;
 }

 ++frame_._idx;

 // If is first
 if (frame_._idx == 1) {
  frame_._sub_part = locals_._ret_part;
  locals_._ret_part.clear_incoming_state_nr();
  locals_._ret_part.clear_outgoing_transitions();
 } else {
  frame_._sub_part.connect_outgoing_transitions_to(*locals_._ret_part.get_incoming_state_nr(), *frame_._state_machine_target);
  frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);
 }

 // If is last
 if (frame_._idx == frame_._index_permutation.size()) {
  locals_._ret_part = frame_._sub_part;
 } else {
  frame_._is_delimiting = frame_._delim_cur_path.has_value();
  locals_._keep_current_frame = true;
 }
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, PermuteFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 Permute const permute(*frame_._expr_cur_path.resolve(args_._ast_root));
 frame_._expr_cur_path = frame_._expr_cur_path.clone_push(0);  // 0th index has the sequence
 frame_._index_permutation_generator = IndexPermutationGenerator(permute.get_min_items(), permute.get_max_items(), permute.get_sequence_length());

 StateNrType state_nr_incoming = frame_._state_machine_target->create_new_state();
 frame_._state_cur = frame_._state_machine_target->get_state(state_nr_incoming);
 frame_._sub_part.set_incoming_state_nr(state_nr_incoming);
}

void process_frame_01(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, PermuteFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 SequenceFrame& seq = *std::get_if<SequenceFrame>(&locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path, *frame_._state_machine_target, frame_._is_hiding)));

 // Set up the permutation for the sequence
 std::ranges::copy(frame_._index_permutation_generator.get_permutation().value(), std::back_inserter(seq._index_permutation));
 seq._is_permuted = true;
}

void process_frame_02(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, PermuteFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 frame_._state_cur->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);

 frame_._idx = 0;
 frame_._index_permutation_generator.advance();

 if (frame_._index_permutation_generator.get_permutation().has_value()) {
  locals_._keep_current_frame = true;
  return;
 }

 locals_._ret_part = frame_._sub_part;
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, PermuteDelimitedFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 Permute const permute(*frame_._expr_cur_path.resolve(args_._ast_root));
 frame_._index_permutation_generator = IndexPermutationGenerator(permute.get_min_items(), permute.get_max_items(), permute.get_sequence_length());

 StateNrType state_nr_incoming = frame_._state_machine_target->create_new_state();
 frame_._state_cur = frame_._state_machine_target->get_state(state_nr_incoming);
 frame_._sub_part.set_incoming_state_nr(state_nr_incoming);
}

void process_frame_01(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, PermuteDelimitedFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 SequenceFrame& seq = *std::get_if<SequenceFrame>(&locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(0), *frame_._state_machine_target, frame_._is_hiding)));

 // Set up the permutation for the sequence
 std::ranges::copy(frame_._index_permutation_generator.get_permutation().value(), std::back_inserter(seq._index_permutation));
 seq._is_permuted = true;

 // Set up the delimiter
 seq._delim_cur_path = frame_._expr_cur_path.clone_push(1);  // 1st index has the delimiter
}

void process_frame_02(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, PermuteDelimitedFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 frame_._state_cur->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);

 frame_._idx = 0;
 frame_._index_permutation_generator.advance();

 if (frame_._index_permutation_generator.get_permutation().has_value()) {
  locals_._keep_current_frame = true;
  return;
 }

 locals_._ret_part = frame_._sub_part;
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, IntersectFrame& frame_) {
 ++frame_._stage;
 locals_._keep_current_frame = true;

 // set up LHS
 locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(0).clone_push(frame_._idx++), frame_._sm_lhs, frame_._is_hiding));
}

void process_frame_01(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, IntersectFrame& frame_) {
 // set up LHS for the first time
 if (frame_._idx == 1) {
  state_machine_part_to_state_machine(locals_._ret_part, frame_._sm_lhs);
  locals_._ret_part.clear_incoming_state_nr();
  locals_._ret_part.clear_outgoing_transitions();
 }

 // set up RHS
 if (frame_._idx < frame_._expr_cur_path.clone_push(0).resolve(args_._ast_root)->get_children_size()) {
  ++frame_._stage;
  locals_._keep_current_frame = true;
  locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(0).clone_push(frame_._idx++), frame_._sm_rhs, frame_._is_hiding));
 } else {
  locals_._ret_part = state_machine_merge(*frame_._state_machine_target, frame_._sm_lhs);
 }
}

void process_frame_02(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, IntersectFrame& frame_) {
 // Finish setting up RHS
 state_machine_part_to_state_machine(locals_._ret_part, frame_._sm_rhs);
 locals_._ret_part.clear_incoming_state_nr();
 locals_._ret_part.clear_outgoing_transitions();

 // intersect LHS and RHS
 frame_._sm_lhs = FsmIntersector::intersect(FsmIntersector::Args{._state_machine_lhs = frame_._sm_lhs, ._state_machine_rhs = std::move(frame_._sm_rhs), ._symbol_kinds_to_follow{SymbolKindCharacter, SymbolKindHiddenCharacter}});
 --frame_._stage;
 locals_._keep_current_frame = true;
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, ChoicesFrame& frame_) {
 StateNrType state_nr_incoming = frame_._state_machine_target->create_new_state();
 frame_._state_cur = frame_._state_machine_target->get_state(state_nr_incoming);
 frame_._sub_part.set_incoming_state_nr(state_nr_incoming);

 locals_._keep_current_frame = true;
 ++frame_._stage;
}

void process_frame_01(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, ChoicesFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(frame_._idx), *frame_._state_machine_target, frame_._is_hiding));
}

void process_frame_02(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, ChoicesFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 frame_._state_cur->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);

 if (frame_._idx < frame_._expr_cur_path.resolve(args_._ast_root)->get_children_size()) {
  locals_._keep_current_frame = true;
  return;
 }

 locals_._ret_part = frame_._sub_part;
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, RepetitionFrame& frame_) {
 frame_._range = RepetitionRange(*frame_._expr_cur_path.clone_push(1).resolve(args_._ast_root));

 if (frame_._range.get_upper() == 0) {
  locals_._ret_part = build_epsilon(args_, *frame_._state_machine_target);
  return;
 }

 StateNrType const state_nr_choices = frame_._state_machine_target->create_new_state();
 frame_._state_choices = frame_._state_machine_target->get_state(state_nr_choices);
 frame_._choices.set_incoming_state_nr(state_nr_choices);

 if (frame_._range.get_lower() == 0) {
  StateMachinePart tmp = build_epsilon(args_, *frame_._state_machine_target);
  frame_._state_choices->add_epsilon_transition(*tmp.get_incoming_state_nr());
  frame_._choices.merge_outgoing_transitions(tmp);
  frame_._range.set_lower(1);
 }

 frame_._idx_max = ((frame_._range.get_upper().value_or(frame_._range.get_lower()) - frame_._range.get_lower() + 1) * (frame_._range.get_upper().value_or(frame_._range.get_lower()) + frame_._range.get_lower())) / 2;

 locals_._keep_current_frame = true;
 ++frame_._stage;
}

void process_frame_01(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, RepetitionFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(0), *frame_._state_machine_target, frame_._is_hiding));
}

void process_frame_02(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, RepetitionFrame& frame_) {
 --frame_._stage;

 if (is_chunk_first(frame_._idx, frame_._idx_chunk, frame_._range)) {
  frame_._chunk = locals_._ret_part;
  locals_._ret_part.clear_incoming_state_nr();
  locals_._ret_part.clear_outgoing_transitions();
 } else {
  frame_._chunk.connect_outgoing_transitions_to(*locals_._ret_part.get_incoming_state_nr(), *frame_._state_machine_target);
  frame_._chunk.merge_outgoing_transitions(locals_._ret_part);
 }

 if (is_chunk_last(frame_._idx, frame_._idx_chunk, frame_._range)) {
  frame_._state_choices->add_epsilon_transition(*frame_._chunk.get_incoming_state_nr());
  frame_._choices.merge_outgoing_transitions(frame_._chunk);
  ++frame_._idx_chunk;
 }

 if (!is_last(frame_._idx, frame_._idx_max)) {
  locals_._keep_current_frame = true;

 } else {
  if (!frame_._range.get_upper().has_value()) {
   StateNrType state_nr_loop_back = frame_._state_machine_target->create_new_state();
   State& state_loop_back = *frame_._state_machine_target->get_state(state_nr_loop_back);
   state_loop_back.add_epsilon_transition(*frame_._choices.get_incoming_state_nr());
   frame_._choices.connect_outgoing_transitions_to(state_nr_loop_back, *frame_._state_machine_target);
   frame_._choices.add_outgoing_epsilon_transition(state_nr_loop_back);
  }

  locals_._ret_part = frame_._choices;
 }

 ++frame_._idx;
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, StringLiteralFrame& frame_) {
 StateNrType state_nr_prev = frame_._state_machine_target->create_new_state();
 State* state_prev = frame_._state_machine_target->get_state(state_nr_prev);
 locals_._ret_part.set_incoming_state_nr(state_nr_prev);

 GenericAst const& expr_cur = *frame_._expr_cur_path.resolve(args_._ast_root);

 std::string const& str_literal = StringLiteral(expr_cur).get_value();

 SymbolKindType const kind = (frame_._is_hiding) ? SymbolKindHiddenCharacter : SymbolKindCharacter;

 for (size_t i = 1; i < str_literal.size(); ++i) {
  StateNrType state_nr_cur = frame_._state_machine_target->create_new_state();
  State* state_cur = frame_._state_machine_target->get_state(state_nr_cur);

  state_prev->add_symbol_transition(Symbol(kind, str_literal[i - 1]), state_nr_cur);
  state_prev = state_cur;
  state_nr_prev = state_nr_cur;
 }

 locals_._ret_part.add_outgoing_symbol_transition(state_nr_prev, Symbol(kind, str_literal.back()));
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, IntegerLiteralFrame& frame_) {
 StateNrType const state_nr_incoming = frame_._state_machine_target->create_new_state();
 locals_._ret_part.set_incoming_state_nr(state_nr_incoming);
 SymbolKindType const kind = (frame_._is_hiding) ? SymbolKindHiddenCharacter : SymbolKindCharacter;
 locals_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(kind, Number(*frame_._expr_cur_path.resolve(args_._ast_root)).get_value()));
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, CharsetFrame& frame_) {
 StateNrType state_nr_incoming = frame_._state_machine_target->create_new_state();
 locals_._ret_part.set_incoming_state_nr(state_nr_incoming);

 GenericAst const& expr_cur = *frame_._expr_cur_path.resolve(args_._ast_root);

 SymbolKindType const kind = (frame_._is_hiding) ? SymbolKindHiddenCharacter : SymbolKindCharacter;
 Charset(expr_cur).get_values().for_each_interval([&](Interval<SymbolValueType> const& interval_) { locals_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, kind, interval_); });
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, EpsilonFrame& frame_) {
 locals_._ret_part = build_epsilon(args_, *frame_._state_machine_target);
}

void process_frame_00(StateMachinePartBuilder::NonterminalBuildingArgs const& args_, Locals& locals_, NonterminalIdentifierFrame& frame_) {
 GenericAst const& expr_cur = *frame_._expr_cur_path.resolve(args_._ast_root);

 std::string const& nonterminal_label = expr_cur.get_string();
 frame_._nonterminal_idx_cur = args_._fn_rev_lookup_nonterminal_label(nonterminal_label);

 auto itr_close = locals_._nonterminal_idx_to_state_nr_close.find(frame_._nonterminal_idx_cur);
 if (itr_close == locals_._nonterminal_idx_to_state_nr_close.end()) {
  itr_close = locals_._nonterminal_idx_to_state_nr_close.emplace(frame_._nonterminal_idx_cur, frame_._state_machine_target->create_new_state()).first;
  State& state_close = *frame_._state_machine_target->get_state(itr_close->second);
  state_close.get_accepts().insert(Interval(frame_._nonterminal_idx_cur));
 }

 frame_._state_nr_open_cur = frame_._state_machine_target->create_new_state();

 auto itr_post_open = locals_._nonterminal_idx_to_state_nr_post_open.find(frame_._nonterminal_idx_cur);
 if (itr_post_open == locals_._nonterminal_idx_to_state_nr_post_open.end()) {
  StateNrType const state_nr_post_open = frame_._state_machine_target->create_new_state();
  locals_._nonterminal_idx_to_state_nr_post_open.emplace(frame_._nonterminal_idx_cur, state_nr_post_open);
  ++frame_._stage;
  locals_._keep_current_frame = true;
  locals_._callstack.emplace_back(construct_frame(args_, args_._fn_lookup_definition(frame_._nonterminal_idx_cur), *frame_._state_machine_target, false));
 } else {
  locals_._ret_part.connect_outgoing_transitions_to(itr_post_open->second, *frame_._state_machine_target);
  locals_._ret_part.set_incoming_state_nr(frame_._state_nr_open_cur);
  locals_._ret_part.add_outgoing_symbol_transition(frame_._state_nr_open_cur, Symbol(SymbolKindClose, frame_._nonterminal_idx_cur));
  State& state_open = *frame_._state_machine_target->get_state(frame_._state_nr_open_cur);
  state_open.add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), itr_post_open->second);
 }
}

void process_frame_01(StateMachinePartBuilder::NonterminalBuildingArgs const& args_, Locals& locals_, NonterminalIdentifierFrame& frame_) {
 State& state_incoming = *frame_._state_machine_target->get_state(frame_._state_nr_open_cur);
 StateNrType const state_nr_close = locals_._nonterminal_idx_to_state_nr_close.find(frame_._nonterminal_idx_cur)->second;
 StateNrType const state_nr_post_open = locals_._nonterminal_idx_to_state_nr_post_open.find(frame_._nonterminal_idx_cur)->second;
 State& state_post_open = *frame_._state_machine_target->get_state(state_nr_post_open);
 state_incoming.add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), state_nr_post_open);
 state_post_open.add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 locals_._ret_part.set_incoming_state_nr(frame_._state_nr_open_cur);
 locals_._ret_part.connect_outgoing_transitions_to(state_nr_close, *frame_._state_machine_target);
 locals_._ret_part.add_outgoing_symbol_transition(frame_._state_nr_open_cur, Symbol(SymbolKindClose, frame_._nonterminal_idx_cur));

 locals_._nonterminal_idx_to_state_nr_post_open.erase(frame_._nonterminal_idx_cur);
}

void process_frame_00(StateMachinePartBuilder::TerminalBuildingArgs const& args_, Locals& locals_, TerminalIdentifierFrame& frame_) {
 GenericAst const& expr_cur = *frame_._expr_cur_path.resolve(args_._ast_root);

 std::string const& terminal_label = expr_cur.get_string();
 frame_._terminal_idx_cur = args_._fn_rev_lookup_terminal_label(terminal_label);

 locals_._terminal_idx_stack.push_back(frame_._terminal_idx_cur);
 if (locals_._terminal_idx_stack_contents.contains(frame_._terminal_idx_cur)) {
  std::string msg = "Terminal '" + terminal_label + "' is recursive: ";
  std::string delim;
  for (size_t const stack_terminal_idx : locals_._terminal_idx_stack) {
   msg += std::exchange(delim, " -> ") + args_._fn_lookup_terminal_label(stack_terminal_idx);
  }
  throw std::runtime_error(msg);
 }

 locals_._terminal_idx_stack_contents.insert(Interval<size_t>(frame_._terminal_idx_cur));

 ++frame_._stage;
 locals_._keep_current_frame = true;
 locals_._callstack.emplace_back(construct_frame(args_, args_._fn_lookup_definition(frame_._terminal_idx_cur), *frame_._state_machine_target, false));
}

void process_frame_01(StateMachinePartBuilder::TerminalBuildingArgs const& args_, Locals& locals_, TerminalIdentifierFrame& frame_) {
 locals_._terminal_idx_stack_contents.erase(Interval<size_t>(frame_._terminal_idx_cur));
 locals_._terminal_idx_stack.pop_back();
}

void process_frame_00(StateMachinePartBuilder::NonterminalBuildingArgs const& args_, Locals& locals_, TerminalIdentifierFrame& frame_) {
 GenericAst const& expr_cur = *frame_._expr_cur_path.resolve(args_._ast_root);
 StateNrType const state_nr_incoming = frame_._state_machine_target->create_new_state();
 locals_._ret_part.set_incoming_state_nr(state_nr_incoming);
 locals_._ret_part.add_outgoing_symbol_transition(state_nr_incoming, Symbol(frame_._is_hiding ? SymbolKindHiddenTerminal : SymbolKindTerminal, args_._fn_rev_lookup_terminal_label(expr_cur.get_string())));
}

void process_frame_00(StateMachinePartBuilder::ArgsBase const& args_, Locals& locals_, HiddenFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(args_, frame_._expr_cur_path.clone_push(0), *frame_._state_machine_target, true));
}

void dispatch(auto const& args_, Locals& locals_, Frame& frame_) {
 switch (std::visit([](auto const& f_) { return f_._stage; }, frame_)) {
  case 0:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_00(static_cast<StateMachinePartBuilder::ArgsBase const&>(args_), locals_, f_); }) {
      process_frame_00(static_cast<StateMachinePartBuilder::ArgsBase const&>(args_), locals_, f_);
     } else if constexpr (requires { process_frame_00(args_, locals_, f_); }) {
      process_frame_00(args_, locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  case 1:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_01(static_cast<StateMachinePartBuilder::ArgsBase const&>(args_), locals_, f_); }) {
      process_frame_01(static_cast<StateMachinePartBuilder::ArgsBase const&>(args_), locals_, f_);
     } else if constexpr (requires { process_frame_01(args_, locals_, f_); }) {
      process_frame_01(args_, locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  case 2:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_02(static_cast<StateMachinePartBuilder::ArgsBase const&>(args_), locals_, f_); }) {
      process_frame_02(static_cast<StateMachinePartBuilder::ArgsBase const&>(args_), locals_, f_);
     } else if constexpr (requires { process_frame_02(args_, locals_, f_); }) {
      process_frame_02(args_, locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  default:
   pmt::unreachable();
 }
}

auto build_common(auto const& args_, Locals& locals_) -> StateMachinePart {
 locals_._callstack.emplace_back(construct_frame(args_, args_._fn_lookup_definition(args_._starting_index), args_._state_machine_dest, false));

 while (!locals_._callstack.empty()) {
  locals_._keep_current_frame = false;
  size_t const frame_idx = locals_._callstack.size() - 1;
  dispatch(args_, locals_, locals_._callstack.back());
  if (!locals_._keep_current_frame) {
   locals_._callstack.erase(locals_._callstack.begin() + frame_idx);
  }
 }

 return std::move(locals_._ret_part);
}

}  // namespace

StateMachinePartBuilder::TerminalBuildingArgs::TerminalBuildingArgs(ArgsBase base_, LabelLookupFn fn_lookup_terminal_label_, ReverseLabelLookupFn fn_rev_lookup_terminal_label_)
 : ArgsBase(std::move(base_))
 , _fn_lookup_terminal_label(std::move(fn_lookup_terminal_label_))
 , _fn_rev_lookup_terminal_label(std::move(fn_rev_lookup_terminal_label_)) {
}

StateMachinePartBuilder::NonterminalBuildingArgs::NonterminalBuildingArgs(ArgsBase base_, LabelLookupFn fn_lookup_nonterminal_label_, ReverseLabelLookupFn fn_rev_lookup_nonterminal_label_, ReverseLabelLookupFn fn_rev_lookup_terminal_label_)
 : ArgsBase(std::move(base_))
 , _fn_lookup_nonterminal_label(std::move(fn_lookup_nonterminal_label_))
 , _fn_rev_lookup_nonterminal_label(std::move(fn_rev_lookup_nonterminal_label_))
 , _fn_rev_lookup_terminal_label(std::move(fn_rev_lookup_terminal_label_)) {
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

}  // namespace pmt::parser::builder