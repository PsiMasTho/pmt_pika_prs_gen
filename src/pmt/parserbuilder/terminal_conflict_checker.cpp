#include "pmt/parserbuilder/terminal_conflict_checker.hpp"

#include "pmt/asserts.hpp"
#include "pmt/util/smct/state_machine_pruner.hpp"
#include "pmt/util/smct/state_machine_determinizer.hpp"
#include "pmt/parserbuilder/state_machine_part_builder.hpp"

#include <unordered_map>
#include <unordered_set>

namespace pmt::parserbuilder {
using namespace pmt::util::smct;
using namespace pmt::util::smrt;
using namespace pmt::base;

namespace {
class Locals {
 public:
  std::unordered_map<StateNrType, StateNrType> _old_to_new;
  std::unordered_map<size_t, std::unordered_set<StateNrType>> _post_close_state_nrs_old;
  StateMachine _state_machine_new;
  std::vector<StateNrType> _pending;
};

auto push_and_visit(TerminalConflictChecker::Args& args_, Locals& locals_, StateNrType state_nr_) -> StateNrType {
 if (auto const itr = locals_._old_to_new.find(state_nr_); itr != locals_._old_to_new.end()) {
  return itr->second;
 }

 StateNrType const state_nr_new = locals_._state_machine_new.create_new_state();
 locals_._old_to_new.emplace(state_nr_, state_nr_new);
 locals_._pending.push_back(state_nr_);
 return state_nr_new;
}

auto take(Locals& locals_) -> StateNrType {
 StateNrType const ret = locals_._pending.back();
 locals_._pending.pop_back();
 return ret;
}

void follow_and_copy_epsilon_transitions(TerminalConflictChecker::Args& args_, Locals& locals_, State const& state_old_, State& state_new_) {
 state_old_.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_old_) {
  state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_));
 });
}

void follow_and_copy_terminal_transitions(TerminalConflictChecker::Args& args_, Locals& locals_, Interval<SymbolType> const& interval_, StateNrType state_nr_next_old_, State& state_new_) {
 StateNrType const state_nr_next_new = push_and_visit(args_, locals_, state_nr_next_old_);

 interval_.for_each_key([&](SymbolType symbol_) {
  SymbolType const value = Symbol(symbol_).get_value();

  StateMachine state_machine_terminal;
  StateMachinePart state_machine_part_terminal = StateMachinePartBuilder::build(
    StateMachinePartBuilder::TerminalBuildingArgs(
      StateMachinePartBuilder::ArgsBase{
        ._ast_root = args_._ast,
        ._dest_state_machine = state_machine_terminal,
        ._fn_lookup_definition = [&](size_t index_) { return args_._grammar_data.lookup_terminal_definition_by_index(index_); },
        ._starting_index = value
      },
      [&](size_t index_) { return args_._grammar_data.lookup_terminal_label_by_index(index_); },
      [&](std::string_view name_) { return args_._grammar_data.lookup_terminal_index_by_label(name_); }
    )
  );

  StateNrType const state_nr_end = state_machine_terminal.create_new_state();
  State* state_end = state_machine_terminal.get_state(state_nr_end);
  state_end->get_accepts().resize(args_._grammar_data._terminal_accepts.size(), false);
  state_end->get_accepts().set(value, true);
  state_machine_part_terminal.connect_outgoing_transitions_to(state_nr_end, state_machine_terminal);

  // Connect the newly created terminal state machine to the new state
  StateNrType const state_nr_terminal = locals_._state_machine_new.get_unused_state_nr();
  StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_terminal, ._state_nr_from_new = state_nr_terminal});
  state_machine_terminal.get_state_nrs().for_each_key([&](StateNrType state_nr_) { locals_._state_machine_new.get_or_create_state(state_nr_) = *state_machine_terminal.get_state(state_nr_); });
  state_new_.add_epsilon_transition(state_nr_terminal);

  // Search for the accepting state and point it to the next state when found
  {
   std::vector<StateNrType> pending;
   IntervalSet<StateNrType> visited;
   pending.push_back(state_nr_terminal);

   while (!pending.empty()) {
    StateNrType const state_nr_cur = pending.back();
    pending.pop_back();

    if (visited.contains(state_nr_cur)) {
     continue;
    }
    visited.insert(Interval(state_nr_cur));

    State& state_cur = *locals_._state_machine_new.get_state(state_nr_cur);
    if (state_cur.get_accepts().size() > value && state_cur.get_accepts().get(value)) {
     state_cur.add_epsilon_transition(state_nr_next_new);
     break;
    }

    state_cur.get_epsilon_transitions().for_each_key([&](StateNrType next_state_nr_) {
     pending.push_back(next_state_nr_);
    });

    state_cur.get_symbol_transitions().for_each_interval([&](StateNrType next_state_nr_, Interval<SymbolType> interval_) {
     pending.push_back(next_state_nr_);
    });
   }
  }
 });
}

void follow_and_copy_open_transitions(TerminalConflictChecker::Args& args_, Locals& locals_, Interval<SymbolType> const& interval_, StateNrType state_nr_next_old_, State& state_new_) {
 state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_));
}

void follow_and_copy_conflict_transitions(TerminalConflictChecker::Args& args_, Locals& locals_, Interval<SymbolType> const& interval_, StateNrType state_nr_next_old_, State& state_new_) {
 state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_));
}

void follow_and_copy_symbol_transitions(TerminalConflictChecker::Args& args_, Locals& locals_, State const& state_old_, State& state_new_) {
  state_old_.get_symbol_transitions().for_each_interval(
   [&](StateNrType state_nr_next_old_, Interval<SymbolType> interval_) {
    SymbolType const kind_max = Symbol(interval_.get_upper()).get_kind();
    for (SymbolType kind_i = Symbol(interval_.get_lower()).get_kind(); kind_i <= kind_max; ++kind_i) {
     Interval<SymbolType> const kind_mask(Symbol(kind_i, 0).get_combined(), Symbol(kind_i, SymbolValueMax).get_combined());
     std::optional<Interval<SymbolType>> const kind_symbol_interval = kind_mask.clone_and(interval_);
     if (!kind_symbol_interval.has_value()) {
      return;
     }

     switch (kind_i) {
       case SymbolKindTerminal: {
        follow_and_copy_terminal_transitions(args_, locals_, *kind_symbol_interval, state_nr_next_old_, state_new_);
       }
       break;
       case SymbolKindOpen: {
        follow_and_copy_open_transitions(args_, locals_, *kind_symbol_interval, state_nr_next_old_, state_new_);
       }
       break;
       case SymbolKindClose: {
        // Do not follow
       }
       break;
       case SymbolKindConflict: {
        follow_and_copy_conflict_transitions(args_, locals_, *kind_symbol_interval, state_nr_next_old_, state_new_);
       }
       break;
       default: {
        pmt::unreachable();
       }
     }
    }
   }
  );
}

void follow_and_copy_post_close_transitions(TerminalConflictChecker::Args& args_, Locals& locals_, State const& state_old_, State& state_new_) {
 Bitset const& accepts = state_old_.get_accepts();
 for (size_t i = 0; i < accepts.size(); ++i) {
  if (!accepts.get(i)) {
   continue;
  }

  auto const itr = locals_._post_close_state_nrs_old.find(i);
  if (itr != locals_._post_close_state_nrs_old.end()) {
   continue;
  }

  for (StateNrType const state_nr_next_old_ : itr->second) {
   state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_));
  }
 }
}

void report_conflicts(TerminalConflictChecker::Args const& args_, Locals const& locals_) {
 // Check for more than one accept in the same state, only report unique conflicts
 std::unordered_set<Bitset> conflicting_accepts;

 locals_._state_machine_new.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *locals_._state_machine_new.get_state(state_nr_);

  if (state.get_accepts().popcnt() <= 1) {
   return;
  }

  conflicting_accepts.insert(state.get_accepts());
 });

 if (conflicting_accepts.empty()) {
  return;
 }

 std::string error_msg = "Terminal conflicts found between: ";
 std::string delim_1;
 for (auto const& accepts : conflicting_accepts) {
  std::string delim_2;
  error_msg += std::exchange(delim_1, ", ") + "{";
  
  for (size_t i = 0; i < accepts.size(); ++i) {
   if (!accepts.get(i)) {
    continue;
   }

   error_msg += std::exchange(delim_2, ", ") + args_._grammar_data.lookup_terminal_label_by_index(i);
  }
  
  error_msg += "}";
 }

 throw std::runtime_error(error_msg);
}

}

void TerminalConflictChecker::check_conflicts(Args args_) {
 Locals locals;

 args_._state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *args_._state_machine.get_state(state_nr_);
  IntervalSet<SymbolType> const close_mask (Interval<SymbolType>(Symbol(SymbolKindClose, 0).get_combined(), Symbol(SymbolKindClose, SymbolValueMax).get_combined()));
  IntervalSet<SymbolType> const close_symbols = state.get_symbols().clone_and(close_mask);
  close_symbols.for_each_key([&](SymbolType symbol_) {
   StateNrType const state_nr_next_old = state.get_symbol_transition(Symbol(symbol_));
   locals._post_close_state_nrs_old[Symbol(symbol_).get_value()].insert(state_nr_next_old);
  });
 });

 StateNrType const state_nr_start_new = push_and_visit(args_, locals, args_._state_nr_from);

 while (!locals._pending.empty()) {
  StateNrType const state_nr_old = take(locals);
  State const& state_old = *args_._state_machine.get_state(state_nr_old);
  StateNrType const state_nr_new = locals._old_to_new.find(state_nr_old)->second;
  State& state_new = *locals._state_machine_new.get_state(state_nr_new);

  follow_and_copy_epsilon_transitions(args_, locals, state_old, state_new);
  follow_and_copy_symbol_transitions(args_, locals, state_old, state_new);
  follow_and_copy_post_close_transitions(args_, locals, state_old, state_new);
 }

 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = locals._state_machine_new, ._state_nr_from = state_nr_start_new});

 report_conflicts(args_, locals);
}

}