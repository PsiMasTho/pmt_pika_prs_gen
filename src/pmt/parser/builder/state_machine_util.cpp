#include "pmt/parser/builder/state_machine_util.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/hash.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/grammar/charset_literal.hpp"
#include "pmt/parser/grammar/grammar.hpp"
#include "pmt/parser/grammar/rule.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/ct/state_machine_determinizer.hpp"
#include "pmt/util/sm/ct/state_machine_minimizer.hpp"
#include "pmt/util/sm/ct/state_machine_part.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <deque>
#include <variant>
#include <vector>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;
using namespace pmt::parser::grammar;

namespace {
class Locals;

class FrameBase {
public:
 RuleExpression const* _expr_cur_path;
 size_t _stage = 0;
 bool _is_hiding : 1 = false;
};

class PassthroughFrame : public FrameBase {
public:
};

class SequenceFrame : public FrameBase {
public:
 StateMachinePart _sub_part;
 RuleExpression const* _delim_cur_path = nullptr;
 std::vector<size_t> _index_permutation;
 size_t _idx = 0;
 bool _is_permuted : 1 = false;
 bool _is_delimiting : 1 = false;
};

/*
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
*/

class ChoiceFrame : public FrameBase {
public:
 StateMachinePart _sub_part;
 State* _state_cur = nullptr;
 size_t _idx = 0;
};

class OneOrMoreFrame : public FrameBase {
public:
};

class CharsetLiteralFrame : public FrameBase {
public:
};

class IdentifierFrame : public FrameBase {
public:
 SymbolValueType _identifier_symbol_cur;
 StateNrType _state_nr_open;
};

class HiddenFrame : public FrameBase {
public:
};

class EpsilonFrame : public FrameBase {
public:
};

using Frame = std::variant<PassthroughFrame, SequenceFrame, ChoiceFrame, OneOrMoreFrame, CharsetLiteralFrame, IdentifierFrame, HiddenFrame, EpsilonFrame>;

class Locals {
public:
 Grammar const& _grammar;
 RuleNameToSymbolFnType const& _rule_name_to_symbol_fn;
 StateMachine& _state_machine_target;
 StateMachinePart _ret_part;
 std::deque<Frame> _callstack;
 std::vector<size_t> _identifier_symbol_stack;
 IntervalSet<size_t> _identifier_symbol_stack_contents;
 bool _keep_current_frame : 1 = false;
};

auto build_epsilon(Locals& locals_) -> StateMachinePart {
 StateNrType const state_nr_incoming = locals_._state_machine_target.create_new_state();
 StateMachinePart ret(state_nr_incoming);
 ret.add_outgoing_epsilon_transition(state_nr_incoming);
 return ret;
}

auto construct_frame(RuleExpression const* expr_cur_path_, bool is_hiding_) -> Frame {
 FrameBase frame_base{._expr_cur_path = expr_cur_path_, ._stage = 0, ._is_hiding = is_hiding_};

 switch (expr_cur_path_->get_tag()) {
  case ClauseBase::Tag::Sequence:
   return SequenceFrame{frame_base};
   break;
  case ClauseBase::Tag::Choice:
   return ChoiceFrame{frame_base};
   break;
  case ClauseBase::Tag::Hidden:
   return HiddenFrame{frame_base};
   break;
  case ClauseBase::Tag::Regular:
   return PassthroughFrame(frame_base);
   break;
  case ClauseBase::Tag::Identifier:
   return IdentifierFrame(frame_base);
   break;
  case ClauseBase::Tag::CharsetLiteral:
   return CharsetLiteralFrame(frame_base);
   break;
  case ClauseBase::Tag::OneOrMore:
   return OneOrMoreFrame(frame_base);
   break;
  case ClauseBase::Tag::Epsilon:
   return EpsilonFrame(frame_base);
   break;
  case ClauseBase::Tag::NotFollowedBy:
  default:
   pmt::unreachable();
 }
}

void process_frame_00(Locals& locals_, PassthroughFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), frame_._is_hiding));
}

void process_frame_00(Locals& locals_, SequenceFrame& frame_) {
 ++frame_._stage;

 if (!frame_._is_permuted) {
  std::generate_n(std::back_inserter(frame_._index_permutation), frame_._expr_cur_path->get_children_size(), [n = 0]() mutable { return n++; });
  frame_._is_permuted = true;
 }

 if (frame_._index_permutation.empty()) {
  locals_._ret_part = build_epsilon(locals_);
  return;
 }

 locals_._keep_current_frame = true;
}

void process_frame_01(Locals& locals_, SequenceFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 if (frame_._is_delimiting) {
  locals_._callstack.emplace_back(construct_frame(frame_._delim_cur_path, frame_._is_hiding));
 } else {
  locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at(frame_._index_permutation[frame_._idx]), frame_._is_hiding));
 }
}

void process_frame_02(Locals& locals_, SequenceFrame& frame_) {
 --frame_._stage;

 if (frame_._is_delimiting) {
  frame_._is_delimiting = false;
  frame_._sub_part.connect_outgoing_transitions_to(*locals_._ret_part.get_incoming_state_nr(), locals_._state_machine_target);
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
  frame_._sub_part.connect_outgoing_transitions_to(*locals_._ret_part.get_incoming_state_nr(), locals_._state_machine_target);
  frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);
 }

 // If is last
 if (frame_._idx == frame_._index_permutation.size()) {
  locals_._ret_part = frame_._sub_part;
 } else {
  frame_._is_delimiting = (frame_._delim_cur_path != nullptr);
  locals_._keep_current_frame = true;
 }
}

void process_frame_00(Locals& locals_, ChoiceFrame& frame_) {
 StateNrType state_nr_incoming = locals_._state_machine_target.create_new_state();
 frame_._state_cur = locals_._state_machine_target.get_state(state_nr_incoming);
 frame_._sub_part.set_incoming_state_nr(state_nr_incoming);

 locals_._keep_current_frame = true;
 ++frame_._stage;
}

void process_frame_01(Locals& locals_, ChoiceFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at(frame_._idx), frame_._is_hiding));
}

void process_frame_02(Locals& locals_, ChoiceFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 frame_._state_cur->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);

 if (frame_._idx < frame_._expr_cur_path->get_children_size()) {
  locals_._keep_current_frame = true;
  return;
 }

 locals_._ret_part = frame_._sub_part;
}

void process_frame_00(Locals& locals_, OneOrMoreFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), frame_._is_hiding));
}

void process_frame_01(Locals& locals_, OneOrMoreFrame& frame_) {
 StateNrType const state_nr_last = locals_._state_machine_target.create_new_state();
 locals_._ret_part.connect_outgoing_transitions_to(state_nr_last, locals_._state_machine_target);
 locals_._state_machine_target.get_state(state_nr_last)->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 locals_._ret_part.add_outgoing_epsilon_transition(state_nr_last);
}

void process_frame_00(Locals& locals_, CharsetLiteralFrame& frame_) {
 StateNrType state_nr_prev = locals_._state_machine_target.create_new_state();
 State* state_prev = locals_._state_machine_target.get_state(state_nr_prev);
 locals_._ret_part.set_incoming_state_nr(state_nr_prev);

 CharsetLiteral const& charset_literal = frame_._expr_cur_path->get_charset_literal();
 SymbolKindType const kind = (frame_._is_hiding) ? SymbolKindHiddenCharacter : charset_literal.is_hidden() ? SymbolKindHiddenCharacter : SymbolKindCharacter;

 for (size_t i = 1; i < charset_literal.size(); ++i) {
  StateNrType state_nr_cur = locals_._state_machine_target.create_new_state();
  State* state_cur = locals_._state_machine_target.get_state(state_nr_cur);

  if (!charset_literal.get_symbol_set_at(i - 1).empty()) {
   charset_literal.get_symbol_set_at(i - 1).for_each_interval([&](Interval<SymbolValueType> interval_) { state_prev->add_symbol_transition(kind, interval_, state_nr_cur); });
  } else {
   state_prev->add_epsilon_transition(state_nr_cur);
  }

  state_prev = state_cur;
  state_nr_prev = state_nr_cur;
 }

 if (!charset_literal.get_symbol_set_at(charset_literal.size() - 1).empty()) {
  charset_literal.get_symbol_set_at(charset_literal.size() - 1).for_each_interval([&](Interval<SymbolValueType> interval_) { locals_._ret_part.add_outgoing_symbol_transition(state_nr_prev, kind, interval_); });
 } else {
  locals_._ret_part.add_outgoing_epsilon_transition(state_nr_prev);
 }
}

void process_frame_00(Locals& locals_, IdentifierFrame& frame_) {
 frame_._identifier_symbol_cur = locals_._rule_name_to_symbol_fn(frame_._expr_cur_path->get_identifier());

 locals_._identifier_symbol_stack.push_back(frame_._identifier_symbol_cur);
 if (locals_._identifier_symbol_stack_contents.contains(frame_._identifier_symbol_cur)) {
  throw std::runtime_error("Recursive regular expression");
 }

 locals_._identifier_symbol_stack_contents.insert(Interval<size_t>(frame_._identifier_symbol_cur));

 ++frame_._stage;
 locals_._keep_current_frame = true;
 Rule const* rule = locals_._grammar.get_rule(frame_._expr_cur_path->get_identifier());
 locals_._callstack.emplace_back(construct_frame(rule->_definition.get(), frame_._is_hiding ? true : rule->_parameters._hide));
 frame_._state_nr_open = locals_._state_machine_target.create_new_state();
}

void process_frame_01(Locals& locals_, IdentifierFrame& frame_) {
 locals_._state_machine_target.get_state(frame_._state_nr_open)->add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), *locals_._ret_part.get_incoming_state_nr());
 locals_._ret_part.set_incoming_state_nr(frame_._state_nr_open);
 StateNrType const state_nr_close = locals_._state_machine_target.create_new_state();
 locals_._ret_part.connect_outgoing_transitions_to(state_nr_close, locals_._state_machine_target);
 locals_._ret_part.add_outgoing_symbol_transition(state_nr_close, Symbol(SymbolKindClose, frame_._identifier_symbol_cur));
}

void process_frame_00(Locals& locals_, HiddenFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), true));
}

void process_frame_00(Locals& locals_, EpsilonFrame& frame_) {
}

void dispatch(Locals& locals_, Frame& frame_) {
 switch (std::visit([](auto const& f_) { return f_._stage; }, frame_)) {
  case 0:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_00(locals_, f_); }) {
      process_frame_00(locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  case 1:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_01(locals_, f_); }) {
      process_frame_01(locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  case 2:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_02(locals_, f_); }) {
      process_frame_02(locals_, f_);
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

auto build_loop(Locals& locals_, RuleExpression const& expr_start_) -> StateMachinePart {
 locals_._callstack.emplace_back(construct_frame(&expr_start_, false));

 while (!locals_._callstack.empty()) {
  locals_._keep_current_frame = false;
  size_t const frame_idx = locals_._callstack.size() - 1;
  dispatch(locals_, locals_._callstack.back());
  if (!locals_._keep_current_frame) {
   locals_._callstack.erase(locals_._callstack.begin() + frame_idx);
  }
 }

 return std::move(locals_._ret_part);
}

}  // namespace

auto terminal_state_machine_hash(StateMachine const& item_, AcceptsEqualityHandling accepts_equality_handling_) -> size_t {
 size_t seed = pmt::base::Hash::Phi64;

 std::vector<StateNrType> pending;
 IntervalSet<StateNrType> visited;

 auto const take = [&]() {
  auto const tmp = pending.back();
  pending.pop_back();
  return tmp;
 };

 auto const push_and_visit = [&](StateNrType state_nr_) {
  if (visited.contains(state_nr_)) {
   return;
  }
  visited.insert(Interval(state_nr_));
  pending.push_back(state_nr_);
 };

 if (item_.get_state(StateNrStart) != nullptr) {
  push_and_visit(StateNrStart);
 }

 while (!pending.empty()) {
  StateNrType const state_nr_cur = take();
  State const& state_cur = *item_.get_state(state_nr_cur);

  // Hash combine the accepts
  switch (accepts_equality_handling_) {
   case AcceptsEqualityHandling::Exact:
    Hash::combine(state_cur.get_accepts(), seed);
    break;
   case AcceptsEqualityHandling::Present:
    Hash::combine(state_cur.get_accepts().empty(), seed);
    break;
  }

  // Hash combine and follow the transitions
  state_cur.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
   Hash::combine(kind_, seed);
   Hash::combine(state_cur.get_symbol_values(kind_), seed);

   state_cur.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_next_, Interval<SymbolValueType> interval_) { push_and_visit(state_nr_next_); });
  });
 }

 return seed;
}

auto terminal_state_machine_eq(StateMachine const& lhs_, StateMachine const& rhs_, AcceptsEqualityHandling accepts_equality_handling_) -> bool {
 std::vector<std::pair<StateNrType, StateNrType>> pending;
 std::unordered_map<StateNrType, StateNrType> lhs_to_rhs;

 auto const take = [&]() {
  auto const tmp = pending.back();
  pending.pop_back();
  return tmp;
 };

 auto const push_and_visit = [&](StateNrType state_nr_lhs_, StateNrType state_nr_rhs_) {
  lhs_to_rhs[state_nr_lhs_] = state_nr_rhs_;
  lhs_to_rhs[state_nr_rhs_] = state_nr_lhs_;
  pending.emplace_back(state_nr_lhs_, state_nr_rhs_);
 };

 auto const cmp_and_follow_transitions = [&](SymbolKindType kind_, State const& state_lhs_, State const& state_rhs_) {
  auto const& transitions_lhs = state_lhs_.get_symbol_transitions(kind_);
  auto const& transitions_rhs = state_rhs_.get_symbol_transitions(kind_);

  if (transitions_lhs.get_keys() != transitions_rhs.get_keys()) {
   return false;
  }

  for (size_t i = 0; i < transitions_lhs.size(); ++i) {
   auto const [state_nr_lhs_next, _1] = transitions_lhs.get_by_index(i);
   auto const [state_nr_rhs_next, _2] = transitions_rhs.get_by_index(i);

   if (auto const itr = lhs_to_rhs.find(state_nr_lhs_next); itr != lhs_to_rhs.end()) {
    if (state_nr_rhs_next != itr->second) {
     return false;
    }
   } else {
    push_and_visit(state_nr_lhs_next, state_nr_rhs_next);
   }
  }

  return true;
 };

 if (!lhs_.get_state(StateNrStart) || !rhs_.get_state(StateNrStart)) {
  return lhs_.get_state(StateNrStart) == rhs_.get_state(StateNrStart);
 }

 push_and_visit(StateNrStart, StateNrStart);

 while (!pending.empty()) {
  auto const [state_nr_lhs_cur, state_nr_rhs_cur] = take();

  State const& state_cur_lhs = *lhs_.get_state(state_nr_lhs_cur);
  State const& state_cur_rhs = *lhs_.get_state(state_nr_rhs_cur);

  // Compare accepts
  switch (accepts_equality_handling_) {
   case AcceptsEqualityHandling::Exact:
    if (state_cur_lhs.get_accepts() != state_cur_rhs.get_accepts()) {
     return false;
    }
    break;
   case AcceptsEqualityHandling::Present:
    if (state_cur_lhs.get_accepts().empty() != state_cur_rhs.get_accepts().empty()) {
     return false;
    }
    break;
  }

  // Compare transitions
  IntervalSet<SymbolKindType> const kinds_lhs = state_cur_lhs.get_symbol_kinds();
  IntervalSet<SymbolKindType> const kinds_rhs = state_cur_rhs.get_symbol_kinds();

  if (kinds_lhs != kinds_rhs) {
   return false;
  }

  for (size_t i = 0; i < kinds_lhs.size(); ++i) {
   Interval<SymbolKindType> interval = kinds_lhs.get_by_index(i);
   for (size_t j = 0; j < interval.get_upper() - interval.get_lower() + 1; ++j) {
    if (!cmp_and_follow_transitions(interval.get_lower() + j, state_cur_lhs, state_cur_rhs)) {
     return false;
    }
   }
  }

  if (!cmp_and_follow_transitions(SymbolKindCharacter, state_cur_lhs, state_cur_rhs) || !cmp_and_follow_transitions(SymbolKindHiddenCharacter, state_cur_lhs, state_cur_rhs)) {
   return false;
  }
 }

 return true;
}

auto state_machine_from_regular_rule(Grammar const& grammar_, RuleExpression const& regular_rule_expression_, AcceptsIndexType accept_, RuleNameToSymbolFnType const& rule_name_to_symbol_fn_) -> StateMachine {
 StateMachine ret;

 Locals locals{._grammar = grammar_, ._rule_name_to_symbol_fn = rule_name_to_symbol_fn_, ._state_machine_target = ret};

 State& state_start = ret.get_or_create_state(StateNrStart);
 StateMachinePart body = build_loop(locals, regular_rule_expression_);
 state_start.add_epsilon_transition(*body.get_incoming_state_nr());
 StateNrType const state_nr_accept = ret.create_new_state();
 body.connect_outgoing_transitions_to(state_nr_accept, ret);
 ret.get_state(state_nr_accept)->get_accepts().insert(Interval(accept_));

 // determinize & minimize
 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = ret});
 StateMachineMinimizer::minimize(ret);

 return ret;
}

}  // namespace pmt::parser::builder