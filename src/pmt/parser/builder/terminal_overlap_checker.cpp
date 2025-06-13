#include "pmt/parser/builder/terminal_overlap_checker.hpp"

#include "pmt/parser/builder/state_machine_part_builder.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/graph_writer.hpp"
#include "pmt/util/sm/ct/state_machine_determinizer.hpp"
#include "pmt/util/sm/ct/state_machine_pruner.hpp"

#include <fstream>
#include <iostream>
#include <unordered_map>

namespace pmt::parser::builder {
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::base;

namespace {
size_t const DOT_FILE_MAX_STATES = 250;

class Locals {
 public:
  std::unordered_map<StateNrType, StateNrType> _old_to_new;
  std::unordered_map<size_t, std::unordered_set<StateNrType>> _post_close_state_nrs_old;
  StateMachine _state_machine_new;
  std::vector<StateNrType> _pending;
  size_t _dotfile_counter = 0;
};

void write_dot(TerminalOverlapChecker::Args const& args_, Locals& locals_, std::string title_, StateMachine const& state_machine_) {
  std::string filename = "conflict_checker_state_machine_" + std::to_string(locals_._dotfile_counter++) + ".dot";
  if (state_machine_.get_state_count() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write of " << filename << " because it has " << state_machine_.get_state_count() << " states, which is more than the limit of " << DOT_FILE_MAX_STATES << '\n';
    return;
  }
  if (state_machine_.get_state_count() == 0) {
    std::cerr << "Skipping dot file write of " << filename << " because it has no states\n";
    return;
  }

  std::ofstream graph_file(filename);
  std::ifstream skel_file("/home/pmt/repos/pmt/skel/pmt/util/smct/state_machine-skel.dot");

  GraphWriter::WriterArgs writer_args{._os_graph = graph_file, ._is_graph_skel = skel_file, ._state_machine = state_machine_};

  GraphWriter::StyleArgs style_args;
  style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {
    return args_._grammar_data.lookup_terminal_label_by_index(accepts_);
  };

  style_args._title = std::move(title_);

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
}

auto push_and_visit(TerminalOverlapChecker::Args& args_, Locals& locals_, StateNrType state_nr_) -> StateNrType {
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

void follow_and_copy_epsilon_transitions(TerminalOverlapChecker::Args& args_, Locals& locals_, State const& state_old_, State& state_new_) {
  state_old_.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_old_) { state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_)); });
}

void follow_and_copy_terminal_transitions(TerminalOverlapChecker::Args& args_, Locals& locals_, Interval<SymbolValueType> const& interval_, StateNrType state_nr_next_old_, State& state_new_) {
  StateNrType const state_nr_next_new = push_and_visit(args_, locals_, state_nr_next_old_);

  interval_.for_each_key([&](SymbolValueType symbol_) {
    StateMachine state_machine_terminal;
    StateMachinePart state_machine_part_terminal = StateMachinePartBuilder::build(StateMachinePartBuilder::TerminalBuildingArgs(
     StateMachinePartBuilder::ArgsBase{._ast_root = args_._ast, ._dest_state_machine = state_machine_terminal, ._fn_lookup_definition = [&](size_t index_) { return args_._grammar_data.lookup_terminal_definition_by_index(index_); }, ._starting_index = symbol_}, [&](size_t index_) { return args_._grammar_data.lookup_terminal_label_by_index(index_); }, [&](std::string_view name_) { return args_._grammar_data.lookup_terminal_index_by_label(name_); }));

    StateNrType const state_nr_end = state_machine_terminal.create_new_state();
    State* state_end = state_machine_terminal.get_state(state_nr_end);
    state_end->get_accepts().insert(Interval(symbol_));
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
        if (state_cur.get_accepts().contains(symbol_)) {
          state_cur.add_epsilon_transition(state_nr_next_new);
          break;
        }

        state_cur.get_epsilon_transitions().for_each_key([&](StateNrType next_state_nr_) { pending.push_back(next_state_nr_); });

        state_cur.get_symbol_transitions(SymbolKindCharacter).for_each_interval([&](StateNrType next_state_nr_, Interval<SymbolValueType> interval_) { pending.push_back(next_state_nr_); });
      }
    }
  });
}

void follow_and_copy_open_transitions(TerminalOverlapChecker::Args& args_, Locals& locals_, Interval<SymbolValueType> const& interval_, StateNrType state_nr_next_old_, State& state_new_) {
  state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_));
}

void follow_and_copy_conflict_transitions(TerminalOverlapChecker::Args& args_, Locals& locals_, Interval<SymbolValueType> const& interval_, StateNrType state_nr_next_old_, State& state_new_) {
  state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_));
}

void follow_and_copy_symbol_transitions(TerminalOverlapChecker::Args& args_, Locals& locals_, State const& state_old_, State& state_new_) {
  state_old_.get_symbol_transitions(SymbolKindTerminal).for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolValueType> interval_) { follow_and_copy_terminal_transitions(args_, locals_, interval_, state_nr_next_old_, state_new_); });

  state_old_.get_symbol_transitions(SymbolKindHiddenTerminal).for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolValueType> interval_) { follow_and_copy_terminal_transitions(args_, locals_, interval_, state_nr_next_old_, state_new_); });

  state_old_.get_symbol_transitions(SymbolKindOpen).for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolValueType> interval_) { follow_and_copy_open_transitions(args_, locals_, interval_, state_nr_next_old_, state_new_); });

  state_old_.get_symbol_transitions(SymbolKindConflict).for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolValueType> interval_) { follow_and_copy_conflict_transitions(args_, locals_, interval_, state_nr_next_old_, state_new_); });
}

void follow_and_copy_post_close_transitions(TerminalOverlapChecker::Args& args_, Locals& locals_, State const& state_old_, State& state_new_) {
  state_old_.get_accepts().for_each_key([&](AcceptsIndexType accepts_index_) {
    auto const itr = locals_._post_close_state_nrs_old.find(accepts_index_);
    if (itr == locals_._post_close_state_nrs_old.end()) {
      return;
    }

    for (StateNrType const state_nr_next_old_ : itr->second) {
      state_new_.add_epsilon_transition(push_and_visit(args_, locals_, state_nr_next_old_));
    }
  });
}

auto gather_overlaps(TerminalOverlapChecker::Args const& args_, Locals const& locals_) -> std::unordered_set<IntervalSet<AcceptsIndexType>> {
  // Check for more than one accept in the same state, only report unique conflicts
  std::unordered_set<IntervalSet<AcceptsIndexType>> conflicting_accepts;

  locals_._state_machine_new.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State const& state = *locals_._state_machine_new.get_state(state_nr_);

    if (state.get_accepts().popcnt() <= 1) {
      return;
    }

    conflicting_accepts.insert(state.get_accepts());
  });

  return conflicting_accepts;
}

}  // namespace

auto TerminalOverlapChecker::find_overlaps(Args args_) -> std::unordered_set<IntervalSet<AcceptsIndexType>> {
  Locals locals;

  args_._state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State const& state = *args_._state_machine.get_state(state_nr_);
    IntervalMap<SymbolValueType, StateNrType> const close_transitions = state.get_symbol_transitions(SymbolKindClose);
    close_transitions.for_each_key([&](StateNrType const state_nr_next_old_, SymbolValueType symbol_) { locals._post_close_state_nrs_old[symbol_].insert(state_nr_next_old_); });
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
  // write_dot(args_, locals, "Terminal Conflict Checker State Machine", locals._state_machine_new);

  return gather_overlaps(args_, locals);
}

}  // namespace pmt::parser::builder