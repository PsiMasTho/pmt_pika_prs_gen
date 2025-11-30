#include "pmt/parser/builder/state_machine_util.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/hash.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/parser/builder/packed_symbol.hpp"
#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/grammar/charset_literal.hpp"
#include "pmt/parser/grammar/grammar.hpp"
#include "pmt/parser/grammar/rule.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/ct/state_machine_determinizer.hpp"
#include "pmt/util/sm/ct/state_machine_minimizer.hpp"
#include "pmt/util/sm/ct/state_machine_part.hpp"
#include "pmt/util/sm/ct/state_machine_pruner.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <variant>
#include <vector>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;
using namespace pmt::parser::grammar;

namespace {
auto get_trace(StateMachine const& state_machine_, StateNrType state_nr_from_, PackedSymbol choice_from_) -> StateMachine {
 StateMachine ret;

 std::unordered_map<StateNrType, StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const push_and_visit = [&](StateNrType state_nr_) -> StateNrType {
  auto itr = visited.find(state_nr_);

  if (itr == visited.end()) {
   StateNrType const state_nr_new = ret.create_new_state();
   itr = visited.emplace(state_nr_, state_nr_new).first;
   pending.push_back(state_nr_);
  }

  return itr->second;
 };

 push_and_visit(state_machine_.get_state(state_nr_from_)->get_symbol_transition(Symbol(SymbolKindOrderedEpsilonOpen, choice_from_.get_packed())));

 while (!pending.empty()) {
  StateNrType const state_nr_old_cur = pending.back();
  pending.pop_back();
  StateNrType const state_nr_new_cur = visited.find(state_nr_old_cur)->second;

  State const& state_old_cur = *state_machine_.get_state(state_nr_old_cur);
  State& state_new_cur = *ret.get_state(state_nr_new_cur);

  state_old_cur.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
   switch (kind_) {
    case SymbolKindCharacter:
    case SymbolKindHiddenCharacter: {
     state_old_cur.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_old_next_, Interval<SymbolValueType> interval_) { state_new_cur.add_symbol_transition(SymbolKindCharacter, interval_, push_and_visit(state_nr_old_next_)); });
    } break;
    case SymbolKindOpen:
    case SymbolKindClose: {
     state_old_cur.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_old_next_, Interval<SymbolValueType> interval_) { state_new_cur.add_epsilon_transition(push_and_visit(state_nr_old_next_)); });
    } break;
    case SymbolKindOrderedEpsilonOpen: {
     state_old_cur.get_symbol_transitions(kind_).for_each_key([&](StateNrType state_nr_old_next_, SymbolValueType symbol_) {
      PackedSymbol const packed_symbol(symbol_);
      if (choice_from_.get_unpacked_lower() != packed_symbol.get_unpacked_lower() || choice_from_.get_unpacked_upper() == packed_symbol.get_unpacked_upper()) {
       state_new_cur.add_epsilon_transition(push_and_visit(state_nr_old_next_));
      }
     });
    } break;
    case SymbolKindOrderedEpsilonClose: {
     state_old_cur.get_symbol_transitions(kind_).for_each_key([&](StateNrType state_nr_old_next_, SymbolValueType symbol_) {
      PackedSymbol const packed_symbol(symbol_);
      if (choice_from_.get_unpacked_lower() != packed_symbol.get_unpacked_lower() || choice_from_.get_unpacked_upper() != packed_symbol.get_unpacked_upper()) {
       state_new_cur.add_epsilon_transition(push_and_visit(state_nr_old_next_));
      } else {
       state_new_cur.get_accepts().insert(Interval<AcceptsIndexType>(1));
      }
     });
    } break;
    default:
     pmt::unreachable();
   }
  });
 }

 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = ret});

 return ret;
}

void prune_with_trace(StateMachine& state_machine_target_, StateMachine const& state_machine_trace_, StateNrType state_nr_from_, PackedSymbol choice_from_) {
 class Transition {
 public:
  StateNrType _state_nr_from;
  Symbol _symbol_from;
 };

 class PendingItem {
 public:
  StateNrType _state_nr_target;
  StateNrType _state_nr_trace;
  Transition _transition_from;
  size_t _id;
  bool _outside = false;
 };

 Symbol const symbol_from(SymbolKindOrderedEpsilonOpen, choice_from_.get_packed());

 size_t id_counter = 0;
 IntervalSet<size_t> ids_for_removal;
 std::unordered_map<size_t, Transition> ids_to_transitions;
 std::unordered_map<StateNrType, IntervalSet<StateNrType>> pending_history;
 std::vector<PendingItem> pending;

 auto const push_and_visit = [&](PendingItem pending_item_) {
  auto itr = pending_history.find(pending_item_._state_nr_target);
  if (itr == pending_history.end()) {
   itr = pending_history.emplace(pending_item_._state_nr_target, IntervalSet<StateNrType>{}).first;
  }

  if (itr->second.contains(pending_item_._state_nr_trace)) {
   return;
  }

  itr->second.insert(Interval(pending_item_._state_nr_trace));
  pending.emplace_back(pending_item_._state_nr_target, pending_item_._state_nr_trace, pending_item_._transition_from, pending_item_._id, pending_item_._outside);
 };

 auto const take = [&]() {
  auto const tmp = pending.back();
  pending.pop_back();
  return tmp;
 };

 // clang-format off
 push_and_visit(
  PendingItem {
   ._state_nr_target = state_machine_target_.get_state(state_nr_from_)->get_symbol_transition(symbol_from),
   ._state_nr_trace = StateNrStart,
   ._transition_from = Transition {
    ._state_nr_from = state_nr_from_,
    ._symbol_from = symbol_from,
   },
   ._id = id_counter++,
  }
 );
 // clang-format on

 while (!pending.empty()) {
  auto const [state_nr_target_cur, state_nr_trace_cur, transition_from_cur, id_cur, outside_cur] = take();

  State* state_target_cur = state_machine_target_.get_state(state_nr_target_cur);
  State const* state_trace_cur = state_machine_trace_.get_state(state_nr_trace_cur);

  size_t id_next = id_cur;

  if (!outside_cur) {
   ids_to_transitions.insert_or_assign(id_cur, transition_from_cur);
  }

  if (!state_trace_cur->get_accepts().empty()) {
   id_next = id_counter++;
   ids_for_removal.insert(Interval(id_cur));
  }

  state_target_cur->get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
   switch (kind_) {
    case SymbolKindCharacter:
    case SymbolKindHiddenCharacter: {
     state_trace_cur->get_symbol_transitions(SymbolKindCharacter).for_each_key([&](StateNrType state_nr_trace_next_, SymbolValueType symbol_value_) {
      Symbol const symbol_cur(kind_, symbol_value_);
      StateNrType const state_nr_target_next_ = state_target_cur->get_symbol_transition(symbol_cur);
      if (state_nr_target_next_ == StateNrSink) {
       return;
      }

      // clang-format off
      push_and_visit(
       PendingItem {
        ._state_nr_target = state_nr_target_next_,
        ._state_nr_trace = state_nr_trace_next_,
        ._transition_from = Transition {
         ._state_nr_from = state_nr_target_cur,
         ._symbol_from = symbol_cur,
        },
        ._id = id_next,
        ._outside = outside_cur
       }
      );
      // clang-format on
     });
    } break;
    case SymbolKindOrderedEpsilonOpen:
    case SymbolKindOrderedEpsilonClose:
    case SymbolKindOpen:
    case SymbolKindClose: {
     bool outside_next = false;
     state_target_cur->get_symbol_transitions(kind_).for_each_key([&](StateNrType state_nr_target_next_, SymbolValueType symbol_value_) {
      if (kind_ == SymbolKindOrderedEpsilonOpen && symbol_value_ == choice_from_.get_packed()) {
       if (outside_cur) {
        id_next = id_next == id_cur ? id_counter++ : id_next;
       }
       outside_next = false;
      } else if (kind_ == SymbolKindOrderedEpsilonClose && symbol_value_ == choice_from_.get_packed()) {
       if (!outside_cur) {
        id_next = id_next == id_cur ? id_counter++ : id_next;
       }
       outside_next = true;
      }

      Symbol const symbol_cur(kind_, symbol_value_);

      // clang-format off
      push_and_visit(
       PendingItem {
        ._state_nr_target = state_nr_target_next_,
        ._state_nr_trace = state_nr_trace_cur,
        ._transition_from = Transition {
         ._state_nr_from = state_nr_target_cur,
         ._symbol_from = symbol_cur,
        },
        ._id = id_next,
        ._outside = outside_next
       }
      );
      // clang-format on
     });
    } break;
    default:
     pmt::unreachable();
   }
  });
 }

 ids_for_removal.for_each_key([&](size_t id_to_remove_) {
  auto const itr = ids_to_transitions.find(id_to_remove_);
  if (itr == ids_to_transitions.end()) {
   return;
  }
  State* const state = state_machine_target_.get_state(itr->second._state_nr_from);
  if (state == nullptr) {
   return;
  }
  state->remove_symbol_transition(itr->second._symbol_from);
 });

 StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_target_});
}

void convert_ordered_epsilon_to_regular_epsilon(StateMachine& state_machine_) {
 state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State& state = *state_machine_.get_state(state_nr_);

  {
   std::vector<Interval<SymbolValueType>> to_remove;
   state.get_symbol_transitions(SymbolKindOrderedEpsilonOpen).for_each_interval([&](StateNrType state_nr_next_, Interval<SymbolValueType> symbols_) {
    to_remove.push_back(symbols_);
    state.add_epsilon_transition(state_nr_next_);
   });

   for (auto const& symbols_ : to_remove) {
    state.remove_symbol_transitions(SymbolKindOrderedEpsilonOpen, symbols_);
   }
  }

  {
   std::vector<Interval<SymbolValueType>> to_remove;
   state.get_symbol_transitions(SymbolKindOrderedEpsilonClose).for_each_interval([&](StateNrType state_nr_next_, Interval<SymbolValueType> symbols_) {
    to_remove.push_back(symbols_);
    state.add_epsilon_transition(state_nr_next_);
   });

   for (auto const& symbols_ : to_remove) {
    state.remove_symbol_transitions(SymbolKindOrderedEpsilonClose, symbols_);
   }
  }
 });
}

void resolve_ordered_epsilon_transitions(StateMachine& state_machine_) {
 IntervalSet<StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const push_and_visit = [&](StateNrType state_nr_) {
  if (!visited.contains(state_nr_)) {
   pending.push_back(state_nr_);
   visited.insert(Interval(state_nr_));
  }
 };

 push_and_visit(StateNrStart);

 while (!pending.empty()) {
  StateNrType const state_nr_cur = pending.back();
  pending.pop_back();

  if (state_machine_.get_state(state_nr_cur) == nullptr) {
   continue;
  }

  IntervalSet<SymbolKindType> const symbol_kinds = state_machine_.get_state(state_nr_cur)->get_symbol_kinds();
  if (symbol_kinds.contains(SymbolKindOrderedEpsilonOpen)) {
   // choice_id -> symbols
   std::unordered_map<size_t, std::vector<SymbolValueType>> choice_id_groups;
   state_machine_.get_state(state_nr_cur)->get_symbol_values(SymbolKindOrderedEpsilonOpen).for_each_key([&](SymbolValueType symbol_) {
    PackedSymbol const symbol_packed(symbol_);
    choice_id_groups[symbol_packed.get_unpacked_lower()].push_back(symbol_packed.get_packed());
   });

   for (auto& [choice_id, choice_id_group] : choice_id_groups) {
    std::ranges::sort(choice_id_group, [](SymbolValueType lhs_, SymbolValueType rhs_) { return PackedSymbol(lhs_).get_unpacked_upper() < PackedSymbol(rhs_).get_unpacked_upper(); });

    for (size_t i = 0; i < choice_id_group.size(); ++i) {
     for (size_t j = choice_id_group.size(); j-- > i + 1;) {
      SymbolValueType const symbol_higher = choice_id_group[j];
      SymbolValueType const symbol_lower = choice_id_group[i];

      // Check that the symbols still exist
      if (state_machine_.get_state(state_nr_cur)->get_symbol_transition(Symbol(SymbolKindOrderedEpsilonOpen, symbol_lower)) == StateNrSink || state_machine_.get_state(state_nr_cur)->get_symbol_transition(Symbol(SymbolKindOrderedEpsilonOpen, symbol_higher)) == StateNrSink) {
       continue;
      }

      StateMachine trace_lower = get_trace(state_machine_, state_nr_cur, PackedSymbol(symbol_lower));

      prune_with_trace(state_machine_, trace_lower, state_nr_cur, PackedSymbol(symbol_higher));
     }
    }
   }
  }

  symbol_kinds.for_each_key([&](SymbolKindType kind_) { state_machine_.get_state(state_nr_cur)->get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_next_, Interval<StateNrType> symbols_) { push_and_visit(state_nr_next_); }); });
 }

 convert_ordered_epsilon_to_regular_epsilon(state_machine_);
}

void remove_dead_states(StateMachine& state_machine_) {
 std::unordered_map<StateNrType, IntervalSet<StateNrType>> reverse_transitions;  // <state_nr_to, state_nrs_from>
 IntervalSet<StateNrType> state_nrs = state_machine_.get_state_nrs();
 IntervalSet<StateNrType> accepting_state_nrs;

 state_nrs.for_each_key([&](StateNrType state_nr_from_) {
  State const& state_from = *state_machine_.get_state(state_nr_from_);
  state_from.get_accepts().empty() ? void() : accepting_state_nrs.insert(Interval(state_nr_from_));

  state_from.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_to_) { reverse_transitions[state_nr_to_].insert(Interval(state_nr_from_)); });

  state_from.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) { state_from.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_to_, Interval<StateNrType> symbols_) { reverse_transitions[state_nr_to_].insert(Interval(state_nr_from_)); }); });
 });

 IntervalSet<StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const push_and_visit = [&](StateNrType state_nr_) {
  if (visited.contains(state_nr_)) {
   return;
  }
  pending.push_back(state_nr_);
  visited.insert(Interval(state_nr_));
 };

 auto const take = [&]() {
  auto const tmp = pending.back();
  pending.pop_back();
  return tmp;
 };

 accepting_state_nrs.for_each_key(push_and_visit);

 while (!pending.empty()) {
  auto const itr = reverse_transitions.find(take());
  if (itr != reverse_transitions.end()) {
   itr->second.for_each_key(push_and_visit);
  }
 }

 state_nrs.inplace_asymmetric_difference(visited);
 state_nrs.for_each_key([&](StateNrType state_nr_unvisited_) { state_machine_.remove_state(state_nr_unvisited_); });

 StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_});
}

enum RegularityKind {
 PegRegular = 0,
 CfgRegular = 1,

 RegularityKindMax = CfgRegular,
};

class Locals;

class FrameBase {
public:
 RuleExpression const* _expr_cur_path;
 size_t _stage = 0;
 bool _is_hiding : 1 = false;
 RegularityKind _regularity_kind : std::bit_width(static_cast<size_t>(RegularityKindMax)) = RegularityKind::PegRegular;
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
 size_t _choice_id = 0;
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

class PegRegularFrame : public FrameBase {
public:
};

class CfgRegularFrame : public FrameBase {
public:
};

class EpsilonFrame : public FrameBase {
public:
};

using Frame = std::variant<PassthroughFrame, SequenceFrame, ChoiceFrame, OneOrMoreFrame, CharsetLiteralFrame, IdentifierFrame, HiddenFrame, PegRegularFrame, CfgRegularFrame, EpsilonFrame>;

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
 size_t _choice_counter = 0;
};

auto build_epsilon(Locals& locals_) -> StateMachinePart {
 StateNrType const state_nr_incoming = locals_._state_machine_target.create_new_state();
 StateMachinePart ret(state_nr_incoming);
 ret.add_outgoing_epsilon_transition(state_nr_incoming);
 return ret;
}

auto construct_frame(RuleExpression const* expr_cur_path_, bool is_hiding_, RegularityKind regularity_kind_) -> Frame {
 FrameBase frame_base{._expr_cur_path = expr_cur_path_, ._stage = 0, ._is_hiding = is_hiding_, ._regularity_kind = regularity_kind_};

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
  case ClauseBase::Tag::PegRegular:
   return PegRegularFrame(frame_base);
   break;
  case ClauseBase::Tag::CfgRegular:
   return CfgRegularFrame(frame_base);
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
 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), frame_._is_hiding, frame_._regularity_kind));
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
  locals_._callstack.emplace_back(construct_frame(frame_._delim_cur_path, frame_._is_hiding, frame_._regularity_kind));
 } else {
  locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at(frame_._index_permutation[frame_._idx]), frame_._is_hiding, frame_._regularity_kind));
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
 frame_._choice_id = locals_._choice_counter++;

 locals_._keep_current_frame = true;
 ++frame_._stage;
}

void process_frame_01(Locals& locals_, ChoiceFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at(frame_._idx), frame_._is_hiding, frame_._regularity_kind));
}

void process_frame_02(Locals& locals_, ChoiceFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 if (frame_._regularity_kind == RegularityKind::PegRegular) {
  frame_._state_cur->add_symbol_transition(Symbol(SymbolKindOrderedEpsilonOpen, PackedSymbol(frame_._choice_id, frame_._idx - 1).get_packed()), *locals_._ret_part.get_incoming_state_nr());
 } else {
  frame_._state_cur->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
 }

 // create a SymbolKindOrderedEpsilonClose transition
 if (frame_._regularity_kind == RegularityKind::PegRegular) {
  StateNrType state_nr_close = locals_._state_machine_target.create_new_state();
  locals_._ret_part.connect_outgoing_transitions_to(state_nr_close, locals_._state_machine_target);
  frame_._sub_part.add_outgoing_symbol_transition(state_nr_close, Symbol(SymbolKindOrderedEpsilonClose, PackedSymbol(frame_._choice_id, frame_._idx - 1).get_packed()));
 } else {
  frame_._sub_part.merge_outgoing_transitions(locals_._ret_part);
 }

 if (frame_._idx < frame_._expr_cur_path->get_children_size()) {
  locals_._keep_current_frame = true;
  return;
 }

 locals_._ret_part = frame_._sub_part;
}

void process_frame_00(Locals& locals_, OneOrMoreFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), frame_._is_hiding, frame_._regularity_kind));
}

void process_frame_01(Locals& locals_, OneOrMoreFrame& frame_) {
 if (frame_._regularity_kind == RegularityKind::CfgRegular) {
  StateNrType const state_nr_last = locals_._state_machine_target.create_new_state();
  locals_._ret_part.connect_outgoing_transitions_to(state_nr_last, locals_._state_machine_target);
  locals_._state_machine_target.get_state(state_nr_last)->add_epsilon_transition(*locals_._ret_part.get_incoming_state_nr());
  locals_._ret_part.add_outgoing_epsilon_transition(state_nr_last);
 } else {
  size_t const choice_id = locals_._choice_counter++;
  StateNrType const state_nr_last_1 = locals_._state_machine_target.create_new_state();
  State& state_last = *locals_._state_machine_target.get_state(state_nr_last_1);
  locals_._ret_part.connect_outgoing_transitions_to(state_nr_last_1, locals_._state_machine_target);
  StateNrType const state_nr_last_2 = locals_._state_machine_target.create_new_state();
  state_last.add_symbol_transition(Symbol(SymbolKindOrderedEpsilonOpen, PackedSymbol(choice_id, 0).get_packed()), *locals_._ret_part.get_incoming_state_nr());
  state_last.add_symbol_transition(Symbol(SymbolKindOrderedEpsilonOpen, PackedSymbol(choice_id, 1).get_packed()), state_nr_last_2);
  locals_._ret_part.add_outgoing_symbol_transition(state_nr_last_1, Symbol(SymbolKindOrderedEpsilonClose, PackedSymbol(choice_id, 0).get_packed()));
  locals_._ret_part.add_outgoing_symbol_transition(state_nr_last_2, Symbol(SymbolKindOrderedEpsilonClose, PackedSymbol(choice_id, 1).get_packed()));
 }
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
 locals_._callstack.emplace_back(construct_frame(rule->_definition.get(), frame_._is_hiding ? true : rule->_parameters._hide, frame_._regularity_kind));
 frame_._state_nr_open = locals_._state_machine_target.create_new_state();
}

void process_frame_01(Locals& locals_, IdentifierFrame& frame_) {
 locals_._state_machine_target.get_state(frame_._state_nr_open)->add_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen), *locals_._ret_part.get_incoming_state_nr());
 locals_._ret_part.set_incoming_state_nr(frame_._state_nr_open);
 StateNrType const state_nr_close = locals_._state_machine_target.create_new_state();
 locals_._ret_part.connect_outgoing_transitions_to(state_nr_close, locals_._state_machine_target);
 locals_._ret_part.add_outgoing_symbol_transition(state_nr_close, Symbol(SymbolKindClose, frame_._identifier_symbol_cur));

 locals_._identifier_symbol_stack_contents.erase(Interval(locals_._identifier_symbol_stack.back()));
 locals_._identifier_symbol_stack.pop_back();
}

void process_frame_00(Locals& locals_, HiddenFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), true, frame_._regularity_kind));
}

void process_frame_00(Locals& locals_, PegRegularFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), frame_._is_hiding, RegularityKind::PegRegular));
}

void process_frame_00(Locals& locals_, CfgRegularFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(frame_._expr_cur_path->get_child_at_back(), frame_._is_hiding, RegularityKind::CfgRegular));
}

void process_frame_00(Locals& locals_, EpsilonFrame& frame_) {
 locals_._ret_part = build_epsilon(locals_);
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
 locals_._callstack.emplace_back(construct_frame(&expr_start_, false, RegularityKind::PegRegular));

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

auto get_accepts_reachable_without_consuming_characters(StateMachine const& state_machine_) -> IntervalSet<AcceptsIndexType> {
 IntervalSet<AcceptsIndexType> ret;

 IntervalSet<StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const push_and_visit = [&](StateNrType state_nr_, auto const&...) {
  if (!visited.contains(state_nr_)) {
   visited.insert(Interval(state_nr_));
   pending.push_back(state_nr_);
  }
 };

 auto const take = [&]() {
  State const* const tmp = state_machine_.get_state(pending.back());
  pending.pop_back();
  return tmp;
 };

 push_and_visit(StateNrStart);

 // Follow all transitions that do not consume a character and collect the accepts
 while (!pending.empty()) {
  State const* const state_cur = take();
  if (state_cur == nullptr) {
   continue;
  }

  ret.inplace_or(state_cur->get_accepts());

  state_cur->get_symbol_transitions(SymbolKindOpen).for_each_interval(push_and_visit);
  state_cur->get_symbol_transitions(SymbolKindClose).for_each_interval(push_and_visit);
  state_cur->get_symbol_transitions(SymbolKindConflict).for_each_interval(push_and_visit);
 }

 return ret;
}

auto create_lookahead_state_machine(StateMachine const& state_machine_) -> StateMachine {
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
 resolve_ordered_epsilon_transitions(ret);
 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = ret});
 remove_dead_states(ret);
 StateMachineMinimizer::minimize(ret);

 return ret;
}

}  // namespace pmt::parser::builder