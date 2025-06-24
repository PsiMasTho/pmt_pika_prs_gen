#include "pmt/parser/builder/parser_lookahead_builder.hpp"

#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/graph_writer.hpp"
#include "pmt/util/sm/ct/state_machine_determinizer.hpp"
#include "pmt/util/sm/ct/state_machine_minimizer.hpp"
#include "pmt/util/sm/ct/state_machine_pruner.hpp"

#include <fstream>
#include <iostream>
#include <unordered_set>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;

namespace {

size_t const DOT_FILE_MAX_STATES = 250;

class Locals {
public:
 StateMachine _lookahead_state_machine;
 size_t _lookahead_dotfile_counter = 0;
};

auto write_lookahead_state_machine_dot(ParserLookaheadBuilder::Args const& args_, Locals& locals_, std::string title_, StateMachine const& state_machine_) -> std::optional<std::string> {
 if (!args_._write_dotfiles) {
  return std::nullopt;
 }

 std::string filename = "lookahead_state_machine_" + std::to_string(locals_._lookahead_dotfile_counter++) + ".dot";
 if (state_machine_.get_state_count() > DOT_FILE_MAX_STATES) {
  std::cerr << "Skipping dot file write of " << filename << " because it has " << state_machine_.get_state_count() << " states, which is more than the limit of " << DOT_FILE_MAX_STATES << '\n';
  return std::nullopt;
 }
 if (state_machine_.get_state_count() == 0) {
  std::cerr << "Skipping dot file write of " << filename << " because it has no states\n";
  return std::nullopt;
 }

 std::ofstream graph_file(filename);
 std::ifstream skel_file("/home/pmt/repos/pmt/skel/pmt/util/sm/ct/state_machine-skel.dot");

 GraphWriter::WriterArgs writer_args{._os_graph = graph_file, ._is_graph_skel = skel_file, ._state_machine = state_machine_};

 GraphWriter::StyleArgs style_args;
 style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {
  return "conflict_" + std::to_string(accepts_);
 };

 style_args._title = std::move(title_);
 style_args._symbols_to_label_fn = [&](SymbolKindType kind_, IntervalSet<SymbolValueType> const& symbols_) -> std::string {
  std::string delim;
  std::string ret;

  switch (kind_) {
   case SymbolKindTerminal:
    symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + args_._fn_lookup_terminal_label(symbol_); });
    break;
   default:
    symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + "unknown"; });
    break;
  }

  return ret;
 };

 style_args._symbol_kind_to_font_flags_fn = [&](SymbolValueType kind_) -> GraphWriter::FontFlags {
  switch (kind_) {
   case SymbolKindTerminal:
    return GraphWriter::FontFlags::Italic;
   default:
    return GraphWriter::FontFlags::None;
  }
 };

 style_args._layout_direction = GraphWriter::LayoutDirection::LeftToRight;

 GraphWriter().write_dot(writer_args, style_args);
 std::cout << "Wrote dot file: " << filename << '\n';
 return filename;
}

auto find_conflicting_state_nrs(StateMachine const& state_machine_) -> IntervalSet<StateNrType> {
 IntervalSet<StateNrType> ret;
 state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *state_machine_.get_state(state_nr_);
  IntervalSet<SymbolKindType> const symbol_kinds = state.get_symbol_kinds();

  // conflict if there is more than one accept or an accept with any symbol
  switch (state.get_accepts().popcnt()) {
   case 0:
    break;
   case 1:
    if (!symbol_kinds.empty()) {
     ret.insert(Interval<StateNrType>(state_nr_));
    }
    return;
   default:
    ret.insert(Interval<StateNrType>(state_nr_));
    return;
  }

  // conflict if an open + close state also has a terminal symbol
  if ((symbol_kinds.contains(SymbolKindClose) || symbol_kinds.contains(SymbolKindOpen)) && (symbol_kinds.contains(SymbolKindTerminal) || symbol_kinds.contains(SymbolKindHiddenTerminal))) {
   ret.insert(Interval<StateNrType>(state_nr_));
  }

  // conflict if the same terminal is hidden and visible
  IntervalSet<SymbolValueType> terminal_symbols = state.get_symbol_transitions(SymbolKindTerminal).get_keys();
  IntervalSet<SymbolValueType> hidden_terminal_symbols = state.get_symbol_transitions(SymbolKindHiddenTerminal).get_keys();
  terminal_symbols.for_each_key([&](SymbolValueType terminal_symbol_) {
   if (hidden_terminal_symbols.contains(terminal_symbol_)) {
    ret.insert(Interval<StateNrType>(state_nr_));
   }
  });
 });

 return ret;
}

auto conflict_symbols_to_string(IntervalSet<SymbolValueType> const& conflict_symbols_) -> std::string {
 static size_t const MAX_REPORTED = 10;
 std::string msg;
 std::string delim;
 size_t count = 0;
 conflict_symbols_.for_each_key([&](SymbolValueType symbol_) {
  if (count > MAX_REPORTED) {
   return;
  }

  if (count++ == MAX_REPORTED) {
   msg += std::exchange(delim, ", ") + "...";
   return;
  }

  msg += std::exchange(delim, ", ") + std::to_string(symbol_);
 });

 return msg;
}

void try_solve_partial_conflict(ParserLookaheadBuilder::Args const& args_, Locals& locals_, StateMachine& state_machine_, IntervalSet<SymbolValueType> conflict_symbols_) {
 std::vector<StateNrType> pending;
 IntervalSet<StateNrType> visited;

 auto const push_and_visit = [&](StateNrType state_nr_) {
  if (visited.contains(state_nr_)) {
   return;
  }

  pending.push_back(state_nr_);
  visited.insert(Interval<StateNrType>(state_nr_));
 };

 push_and_visit(StateNrStart);

 while (!pending.empty()) {
  StateNrType const state_nr_cur = pending.back();
  State& state_cur = *state_machine_.get_state(state_nr_cur);
  pending.pop_back();

  assert(state_cur.get_epsilon_transitions().empty());

  switch (state_cur.get_accepts().popcnt()) {
   case 1:
    state_cur.clear_symbol_transitions();
    continue;
   default:
    if (state_cur.get_symbol_transitions(SymbolKindTerminal).empty()) {
     std::optional<std::string> const filename = write_lookahead_state_machine_dot(args_, locals_, "Partial conflict state machine unsolved", state_machine_);
     std::string const msg = "Error while building lookahead for conflict nrs: {" + conflict_symbols_to_string(conflict_symbols_) + "}, lookahead state nr " + std::to_string(state_nr_cur) + " has multiple accepts and no terminal symbols" + (filename.has_value() ? ", see " + *filename : "");
     throw std::runtime_error(msg);
    }
    state_cur.get_accepts().clear();
   case 0:
    state_cur.get_symbol_transitions(SymbolKindTerminal).for_each_interval([&](StateNrType const state_nr_next_, Interval<SymbolValueType> const) { push_and_visit(state_nr_next_); });
    break;
  }
 }

 StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_});

 // check that all conflict symbols exist at least once
 state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *state_machine_.get_state(state_nr_);
  if (!state.get_accepts().empty()) {
   conflict_symbols_.erase(Interval<SymbolValueType>(state.get_accepts().get_by_index(0).get_lower()));
  }
 });

 if (!conflict_symbols_.empty()) {
  std::optional<std::string> const filename = write_lookahead_state_machine_dot(args_, locals_, "Partial conflict state machine unsolved", state_machine_);
  std::string msg = "Error while building lookahead for conflict nrs: {" + conflict_symbols_to_string(conflict_symbols_) + "}, no unique way to resolve: ";
  std::string delim;
  conflict_symbols_.for_each_key([&](SymbolValueType symbol_) { msg += std::exchange(delim, ", ") + "conflict_" + std::to_string(symbol_); });
  if (filename.has_value()) {
   msg += ", see " + *filename;
  }
  throw std::runtime_error(msg);
 }
}

void setup_lookahead_state_machine(ParserLookaheadBuilder::Args const& args_, Locals& locals_) {
 if (args_._conflicting_state_nrs.empty()) {
  return;
 }

 std::unordered_map<SymbolValueType, std::unordered_set<StateNrType>> post_close_state_nrs;

 args_._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *args_._parser_state_machine.get_state(state_nr_);
  IntervalMap<SymbolValueType, StateNrType> const close_symbols = state.get_symbol_transitions(SymbolKindClose);
  close_symbols.for_each_key([&](StateNrType const state_nr_next_, SymbolValueType const symbol_) { post_close_state_nrs[symbol_].insert(state_nr_next_); });
 });

 locals_._lookahead_state_machine.get_or_create_state(StateNrStart);

 args_._conflicting_state_nrs.for_each_key([&](StateNrType state_nr_conflicting_) {
  State const& state_conflicting = *args_._parser_state_machine.get_state(state_nr_conflicting_);

  StateMachine partial_conflict_state_machine;
  StateNrType const state_nr_partial_start = partial_conflict_state_machine.create_new_state();

  state_conflicting.get_symbol_transitions(SymbolKindConflict).for_each_key([&](StateNrType const state_nr_post_conflcit_, SymbolValueType const conflict_symbol_) {
   StateMachine state_machine_lookahead;

   std::vector<StateNrType> pending;
   IntervalMap<StateNrType, StateNrType> visited;

   auto const push_and_visit = [&](StateNrType state_nr_) -> StateNrType {
    if (StateNrType const* ptr = visited.find(state_nr_); ptr != nullptr) {
     return *ptr;
    }

    StateNrType const state_nr_new = state_machine_lookahead.create_new_state();

    pending.push_back(state_nr_);
    visited.insert(Interval<StateNrType>(state_nr_), state_nr_new);
    return state_nr_new;
   };

   push_and_visit(state_nr_post_conflcit_);

   while (!pending.empty()) {
    StateNrType const state_nr_cur_old = pending.back();
    pending.pop_back();
    StateNrType const state_nr_cur_new = *visited.find(state_nr_cur_old);
    State const& state_old = *args_._parser_state_machine.get_state(state_nr_cur_old);
    State& state_new = state_machine_lookahead.get_or_create_state(state_nr_cur_new);

    state_new.get_accepts().insert(Interval<AcceptsIndexType>(conflict_symbol_));

    assert(state_old.get_epsilon_transitions().empty());

    // SymbolKindTerminal
    state_old.get_symbol_transitions(SymbolKindTerminal).for_each_interval([&](StateNrType const state_nr_next_old_, Interval<SymbolValueType> const interval_) { state_new.add_symbol_transition(SymbolKindTerminal, interval_, push_and_visit(state_nr_next_old_)); });

    // SymbolKindHiddenTerminal
    state_old.get_symbol_transitions(SymbolKindHiddenTerminal).for_each_interval([&](StateNrType const state_nr_next_old_, Interval<SymbolValueType> const interval_) { state_new.add_symbol_transition(SymbolKindTerminal, interval_, push_and_visit(state_nr_next_old_)); });

    // SymbolKindOpen
    state_old.get_symbol_transitions(SymbolKindOpen).for_each_interval([&](StateNrType const state_nr_next_old_, Interval<SymbolValueType> const interval_) { state_new.add_epsilon_transition(push_and_visit(state_nr_next_old_)); });

    // SymbolKindClose
    state_old.get_accepts().for_each_key([&](AcceptsIndexType const accept_) {
     auto const itr = post_close_state_nrs.find(Symbol(SymbolKindClose, accept_).get_value());
     if (itr != post_close_state_nrs.end()) {
      for (StateNrType const state_nr_next_old : itr->second) {
       StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
       state_new.add_epsilon_transition(state_nr_next_new);
      }
     }
    });

    // SymbolKindConflict
    {
     IntervalSet<SymbolValueType> conflict_symbols = state_old.get_symbol_transitions(SymbolKindConflict).get_keys();
     IntervalSet<SymbolValueType> skip_conflict_symbols = state_conflicting.get_symbol_transitions(SymbolKindConflict).get_keys();
     skip_conflict_symbols.erase(Interval<SymbolValueType>(conflict_symbol_));
     conflict_symbols.inplace_asymmetric_difference(skip_conflict_symbols);
     conflict_symbols.for_each_key([&](SymbolValueType symbol_) {
      StateNrType const state_nr_next_old = state_old.get_symbol_transition(Symbol(SymbolKindConflict, symbol_));
      state_new.add_epsilon_transition(push_and_visit(state_nr_next_old));
     });
    }
   }

   // merge this state machine into the partial conflict state machine
   StateNrType const state_nr_lookahead_start = partial_conflict_state_machine.get_unused_state_nr();
   StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_lookahead, ._state_nr_from_new = state_nr_lookahead_start});
   partial_conflict_state_machine.get_state(state_nr_partial_start)->add_epsilon_transition(state_nr_lookahead_start);

   state_machine_lookahead.get_state_nrs().for_each_key([&](StateNrType state_nr_) { partial_conflict_state_machine.get_or_create_state(state_nr_) = *state_machine_lookahead.get_state(state_nr_); });
  });

  StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = partial_conflict_state_machine});
  try_solve_partial_conflict(args_, locals_, partial_conflict_state_machine, state_conflicting.get_symbol_transitions(SymbolKindConflict).get_keys());

  StateNrType const state_nr_prune_start = locals_._lookahead_state_machine.get_unused_state_nr();
  StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = partial_conflict_state_machine, ._state_nr_from_new = state_nr_prune_start});
  locals_._lookahead_state_machine.get_state(StateNrStart)->add_epsilon_transition(state_nr_prune_start);
  partial_conflict_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) { locals_._lookahead_state_machine.get_or_create_state(state_nr_) = *partial_conflict_state_machine.get_state(state_nr_); });
 });
}

}  // namespace

auto ParserLookaheadBuilder::build(Args args_) -> StateMachine {
 Locals locals;

 setup_lookahead_state_machine(args_, locals);
 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = locals._lookahead_state_machine});
 StateMachineMinimizer::minimize(locals._lookahead_state_machine);
 write_lookahead_state_machine_dot(args_, locals, "Final lookahead state machine", locals._lookahead_state_machine);

 return std::move(locals._lookahead_state_machine);
}

auto ParserLookaheadBuilder::extract_conflicts(StateMachine& state_machine_) -> IntervalSet<StateNrType> {
 IntervalSet<StateNrType> const ret = find_conflicting_state_nrs(state_machine_);

 size_t conflict_counter = 0;

 // extract the conflicts
 ret.for_each_key([&](StateNrType state_nr_) {
  // create conflict transitions to the newly created states later
  IntervalSet<StateNrType> state_nrs_new;
  State& state_old = *state_machine_.get_state(state_nr_);
  // every accept needs to get a new state
  state_old.get_accepts().for_each_key([&](size_t const accept_) {
   StateNrType const state_nr_new = state_machine_.create_new_state();
   state_nrs_new.insert(Interval<StateNrType>(state_nr_new));
   State& state_new = *state_machine_.get_state(state_nr_new);
   state_new.get_accepts().insert(Interval<AcceptsIndexType>(accept_));
  });
  state_old.get_accepts().clear();

  // all terminal symbols need to be moved to another single new state
  IntervalMap<SymbolValueType, StateNrType> const terminal_symbols = state_old.get_symbol_transitions(SymbolKindTerminal);

  if (!terminal_symbols.empty()) {
   StateNrType const state_nr_new = state_machine_.create_new_state();
   state_nrs_new.insert(Interval<StateNrType>(state_nr_new));
   State& state_new = *state_machine_.get_state(state_nr_new);
   terminal_symbols.for_each_interval([&](StateNrType const state_nr_next_, Interval<SymbolValueType> const interval_) { state_new.add_symbol_transition(SymbolKindTerminal, interval_, state_nr_next_); });
  }

  // all hidden terminal symbols need to be moved to another single new state
  IntervalMap<SymbolValueType, StateNrType> const hidden_terminal_symbols = state_old.get_symbol_transitions(SymbolKindHiddenTerminal);

  if (!hidden_terminal_symbols.empty()) {
   StateNrType const state_nr_new_hidden = state_machine_.create_new_state();
   state_nrs_new.insert(Interval<StateNrType>(state_nr_new_hidden));
   State& state_new_hidden = *state_machine_.get_state(state_nr_new_hidden);
   hidden_terminal_symbols.for_each_interval([&](StateNrType const state_nr_next_, Interval<SymbolValueType> const interval_) { state_new_hidden.add_symbol_transition(SymbolKindHiddenTerminal, interval_, state_nr_next_); });
  }

  // all open and close symbols need to be moved to another single new state
  IntervalMap<SymbolValueType, StateNrType> const close_symbols = state_old.get_symbol_transitions(SymbolKindClose);

  if (!close_symbols.empty()) {
   IntervalMap<SymbolValueType, StateNrType> const open_symbols = state_old.get_symbol_transitions(SymbolKindOpen);
   assert(open_symbols.size() == 1);
   StateNrType const state_nr_new_open_close = state_machine_.create_new_state();
   state_nrs_new.insert(Interval<StateNrType>(state_nr_new_open_close));
   State& state_new_open_close = *state_machine_.get_state(state_nr_new_open_close);
   close_symbols.for_each_interval([&](StateNrType const state_nr_next_, Interval<SymbolValueType> const interval_) { state_new_open_close.add_symbol_transition(SymbolKindClose, interval_, state_nr_next_); });

   open_symbols.for_each_interval([&](StateNrType const state_nr_next_, Interval<SymbolValueType> const interval_) { state_new_open_close.add_symbol_transition(SymbolKindOpen, interval_, state_nr_next_); });
  }

  // clear all symbols from the old state
  state_old.clear_symbol_transitions();

  // connect the old state to the new with conflict symbols
  state_nrs_new.for_each_key([&](StateNrType state_nr_new_) { state_old.add_symbol_transition(Symbol(SymbolKindConflict, conflict_counter++), state_nr_new_); });
 });

 return ret;
}
}  // namespace pmt::parser::builder