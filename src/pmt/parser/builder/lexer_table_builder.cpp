#include "pmt/parser/builder/lexer_table_builder.hpp"

#include "pmt/base/algo.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/parser/builder/state_machine_part_builder.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/grammar/grammar_data.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/graph_writer.hpp"
#include "pmt/util/sm/ct/state_machine_determinizer.hpp"
#include "pmt/util/sm/ct/state_machine_minimizer.hpp"
#include "pmt/util/sm/ct/state_machine_part.hpp"
#include "pmt/util/sm/ct/state_machine_pruner.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <fstream>
#include <iostream>

namespace pmt::parser::builder {
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;
using namespace pmt::base;
using namespace pmt::parser::grammar;

namespace {}

auto LexerTableBuilder::build(GenericAst const& ast_, GrammarData const& grammar_data_) -> LexerTables {
  _ast = &ast_;
  _grammar_data = &grammar_data_;
  setup_whitespace_state_machine();
  setup_terminal_state_machines();
  setup_comment_state_machines();
  setup_linecount_state_machine();
  loop_back_comment_close_state_machines();
  merge_comment_state_machines_into_result();
  merge_whitespace_state_machine_into_result();
  setup_start_and_eoi_states();
  connect_terminal_state_machines();
  loop_back_linecount_state_machine();
  fill_terminal_data();
  StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = _lexer_state_machine});
  StateMachineMinimizer::minimize(_lexer_state_machine);
  write_dot("lexer_state_machine.dot", "Final tables", _lexer_state_machine);

  StateMachineMinimizer::minimize(_linecount_state_machine);
  write_dot("linecount_state_machine.dot", "Linecount tables", _linecount_state_machine);

  validate_result();

  _result_tables.set_lexer_state_machine(std::move(_lexer_state_machine));
  _result_tables.set_linecount_state_machine(std::move(_linecount_state_machine));

  return std::move(_result_tables);
}

void LexerTableBuilder::setup_whitespace_state_machine() {
  if (_grammar_data->_whitespace_definition.empty()) {
    return;
  }

  size_t const index_whitespace = _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size();
  StateMachinePart fsm_part_whitespace = StateMachinePartBuilder::build(StateMachinePartBuilder::TerminalBuildingArgs(
   StateMachinePartBuilder::ArgsBase{._ast_root = *_ast, ._dest_state_machine = _whitespace_state_machine, ._fn_lookup_definition = [this](size_t index_) { return _grammar_data->lookup_terminal_definition_by_index(index_); }, ._starting_index = index_whitespace}, [this](size_t index_) { return _grammar_data->lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return _grammar_data->lookup_terminal_index_by_label(name_); }));
  StateNrType const state_nr_end = _whitespace_state_machine.create_new_state();
  _whitespace_state_machine.get_state(state_nr_end)->get_accepts().insert(Interval<AcceptsIndexType>(0));
  fsm_part_whitespace.connect_outgoing_transitions_to(state_nr_end, _whitespace_state_machine);
  StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = _whitespace_state_machine});
}

void LexerTableBuilder::setup_terminal_state_machines() {
  for (size_t i = 0; i < _grammar_data->_terminal_accepts.size(); ++i) {
    if (!_grammar_data->_terminal_accepts[i]._accepted || _grammar_data->_terminal_accepts[i]._label == GrammarData::LABEL_EOI || _grammar_data->_terminal_accepts[i]._label == GrammarData::TERMINAL_LABEL_START || _grammar_data->_terminal_accepts[i]._label == GrammarData::TERMINAL_LABEL_LINECOUNT) {
      continue;
    }

    StateMachine state_machine_terminal;
    StateMachinePart state_machine_part_terminal = StateMachinePartBuilder::build(StateMachinePartBuilder::TerminalBuildingArgs(
     StateMachinePartBuilder::ArgsBase{._ast_root = *_ast, ._dest_state_machine = state_machine_terminal, ._fn_lookup_definition = [this](size_t index_) { return _grammar_data->lookup_terminal_definition_by_index(index_); }, ._starting_index = i}, [this](size_t index_) { return _grammar_data->lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return _grammar_data->lookup_terminal_index_by_label(name_); }));
    StateNrType const state_nr_end = state_machine_terminal.create_new_state();
    State& state_end = *state_machine_terminal.get_state(state_nr_end);
    state_end.get_accepts().insert(Interval<AcceptsIndexType>(i));
    state_machine_part_terminal.connect_outgoing_transitions_to(state_nr_end, state_machine_terminal);
    StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = state_machine_terminal});
    _terminal_state_machines.push_back(std::move(state_machine_terminal));
  }
}

void LexerTableBuilder::setup_comment_state_machines() {
  for (size_t i = 0; i < _grammar_data->_comment_open_definitions.size(); ++i) {
    size_t const index_comment_open = _grammar_data->_terminal_accepts.size() + i;

    StateMachine state_machine_comment_open;
    StateMachinePart state_machine_part_comment_open = StateMachinePartBuilder::build(StateMachinePartBuilder::TerminalBuildingArgs(
     StateMachinePartBuilder::ArgsBase{._ast_root = *_ast, ._dest_state_machine = state_machine_comment_open, ._fn_lookup_definition = [this](size_t index_) { return _grammar_data->lookup_terminal_definition_by_index(index_); }, ._starting_index = index_comment_open}, [this](size_t index_) { return _grammar_data->lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return _grammar_data->lookup_terminal_index_by_label(name_); }));
    StateNrType const state_nr_end = state_machine_comment_open.create_new_state();
    state_machine_comment_open.get_state(state_nr_end)->get_accepts().insert(Interval<AcceptsIndexType>(0));
    state_machine_part_comment_open.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_open);
    StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = state_machine_comment_open});
    _comment_open_state_machines.push_back(std::move(state_machine_comment_open));
  }

  for (size_t i = 0; i < _grammar_data->_comment_close_definitions.size(); ++i) {
    size_t const index_comment_close = _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + i;

    StateMachine state_machine_comment_close;
    StateMachinePart state_machine_part_comment_close = StateMachinePartBuilder::build(StateMachinePartBuilder::TerminalBuildingArgs(
     StateMachinePartBuilder::ArgsBase{._ast_root = *_ast, ._dest_state_machine = state_machine_comment_close, ._fn_lookup_definition = [this](size_t index_) { return _grammar_data->lookup_terminal_definition_by_index(index_); }, ._starting_index = index_comment_close}, [this](size_t index_) { return _grammar_data->lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return _grammar_data->lookup_terminal_index_by_label(name_); }));
    StateNrType const state_nr_end = state_machine_comment_close.create_new_state();
    state_machine_comment_close.get_state(state_nr_end)->get_accepts().insert(Interval<AcceptsIndexType>(0));
    state_machine_part_comment_close.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_close);
    StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = state_machine_comment_close});
    _comment_close_state_machines.push_back(std::move(state_machine_comment_close));
  }
}

void LexerTableBuilder::setup_linecount_state_machine() {
  if (_grammar_data->_linecount_definition.empty()) {
    setup_default_linecount_state_machine();
    return;
  }

  size_t const index_linecount = _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size() + 1;
  StateMachinePart fsm_part_linecount = StateMachinePartBuilder::build(StateMachinePartBuilder::TerminalBuildingArgs(
   StateMachinePartBuilder::ArgsBase{._ast_root = *_ast, ._dest_state_machine = _linecount_state_machine, ._fn_lookup_definition = [this](size_t index_) { return _grammar_data->lookup_terminal_definition_by_index(index_); }, ._starting_index = index_linecount}, [this](size_t index_) { return _grammar_data->lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return _grammar_data->lookup_terminal_index_by_label(name_); }));
  StateNrType const state_nr_end = _linecount_state_machine.create_new_state();
  State& state_end = *_linecount_state_machine.get_state(state_nr_end);
  fsm_part_linecount.connect_outgoing_transitions_to(state_nr_end, _linecount_state_machine);
  size_t const index_linecount_terminal = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_LINECOUNT, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  state_end.get_accepts().insert(Interval<AcceptsIndexType>(index_linecount_terminal));
  StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = _linecount_state_machine});
}

void LexerTableBuilder::setup_default_linecount_state_machine() {
  State& state_start = _linecount_state_machine.get_or_create_state(StateNrStart);
  StateNrType const state_nr_end = _linecount_state_machine.create_new_state();
  State& state_end = *_linecount_state_machine.get_state(state_nr_end);
  size_t const index_linecount_terminal = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_LINECOUNT, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  state_end.get_accepts().insert(Interval<AcceptsIndexType>(index_linecount_terminal));
  state_start.add_symbol_transition(Symbol(SymbolKindCharacter, '\n'), state_nr_end);
}

void LexerTableBuilder::loop_back_comment_close_state_machines() {
  for (size_t i = 0; i < _comment_close_state_machines.size(); ++i) {
    StateMachine& state_machine_comment_close = _comment_close_state_machines[i];

    std::vector<StateNrType> pending;
    std::unordered_set<StateNrType> visited;

    auto const push_and_visit = [&pending, &visited](StateNrType state_nr_) {
      if (visited.contains(state_nr_)) {
        return;
      }

      pending.push_back(state_nr_);
      visited.insert(state_nr_);
    };

    push_and_visit(StateNrStart);

    while (!pending.empty()) {
      StateNrType const state_nr_cur = pending.back();
      pending.pop_back();

      State& state_cur = *state_machine_comment_close.get_state(state_nr_cur);

      if (state_cur.get_accepts().empty()) {
        IntervalSet<SymbolValueType> const alphabet(Interval<SymbolValueType>(0, UCHAR_MAX));
        alphabet.clone_asymmetric_difference(state_cur.get_symbol_values(SymbolKindCharacter)).for_each_interval([&](Interval<SymbolValueType> const& interval_) { state_cur.add_symbol_transition(SymbolKindCharacter, interval_, StateNrStart); });

        state_cur.get_symbol_values(SymbolKindCharacter).for_each_key([&](SymbolValueType symbol_) { push_and_visit(state_cur.get_symbol_transition(Symbol(SymbolKindCharacter, symbol_))); });
      }
    }
  }
}

void LexerTableBuilder::merge_comment_state_machines_into_result() {
  _lexer_state_machine.get_or_create_state(StateNrStart);

  for (size_t i = 0; i < _comment_open_state_machines.size(); ++i) {
    StateMachine& state_machine_comment_open = _comment_open_state_machines[i];
    StateNrType const state_nr_comment_open = _lexer_state_machine.create_new_state();
    StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_comment_open, ._state_nr_from_new = state_nr_comment_open});

    StateMachine& state_machine_comment_close = _comment_close_state_machines[i];
    StateNrType const state_nr_comment_close = state_nr_comment_open + state_machine_comment_close.get_state_count();
    StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_comment_close, ._state_nr_from_new = state_nr_comment_close});

    _lexer_state_machine.get_state(StateNrStart)->add_epsilon_transition(state_nr_comment_open);

    state_machine_comment_open.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
      State& state = *state_machine_comment_open.get_state(state_nr_);

      if (state.get_accepts().popcnt() != 0) {
        state.get_accepts().clear();
        state.add_epsilon_transition(state_nr_comment_close);
      }

      _lexer_state_machine.get_or_create_state(state_nr_) = state;
    });

    state_machine_comment_close.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
      State& state = *state_machine_comment_close.get_state(state_nr_);

      if (state.get_accepts().popcnt() != 0) {
        state.get_accepts().clear();
        state.add_epsilon_transition(StateNrStart);
      }

      _lexer_state_machine.get_or_create_state(state_nr_) = state;
    });
  }
}

void LexerTableBuilder::merge_whitespace_state_machine_into_result() {
  if (_grammar_data->_whitespace_definition.empty()) {
    return;
  }

  StateNrType const state_nr_whitespace = _lexer_state_machine.create_new_state();
  StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = _whitespace_state_machine, ._state_nr_from_new = state_nr_whitespace});
  _lexer_state_machine.get_state(StateNrStart)->add_epsilon_transition(state_nr_whitespace);
  _whitespace_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State& state = *_whitespace_state_machine.get_state(state_nr_);
    if (state.get_accepts().popcnt() != 0) {
      state.get_accepts().clear();
      state.add_epsilon_transition(StateNrStart);
    }
    _lexer_state_machine.get_or_create_state(state_nr_) = state;
  });
}

void LexerTableBuilder::setup_start_and_eoi_states() {
  State& state_start = _lexer_state_machine.get_or_create_state(StateNrStart);
  size_t const index_start = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_START, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  state_start.get_accepts().insert(Interval<AcceptsIndexType>(index_start));

  StateNrType const state_nr_eoi = _lexer_state_machine.create_new_state();
  State& state_eoi = *_lexer_state_machine.get_state(state_nr_eoi);
  size_t const index_eoi = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::LABEL_EOI, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  state_eoi.get_accepts().insert(Interval<AcceptsIndexType>(index_eoi));

  state_start.add_symbol_transition(Symbol(SymbolKindCharacter, SymbolValueEoi), state_nr_eoi);
}

void LexerTableBuilder::connect_terminal_state_machines() {
  State& state_start = *_lexer_state_machine.get_state(StateNrStart);

  for (size_t i = 0; i < _terminal_state_machines.size(); ++i) {
    StateMachine& state_machine_terminal = _terminal_state_machines[i];
    StateNrType const state_nr_terminal = _lexer_state_machine.get_unused_state_nr();
    StateMachinePruner::prune(StateMachinePruner::Args{._state_machine = state_machine_terminal, ._state_nr_from_new = state_nr_terminal});

    state_machine_terminal.get_state_nrs().for_each_key([&](StateNrType state_nr_) { _lexer_state_machine.get_or_create_state(state_nr_) = *state_machine_terminal.get_state(state_nr_); });

    state_start.add_epsilon_transition(state_nr_terminal);
  }
}

void LexerTableBuilder::loop_back_linecount_state_machine() {
  IntervalSet<SymbolValueType> alphabet(Interval<SymbolValueType>(0, UCHAR_MAX));
  alphabet.insert(Interval<SymbolValueType>(SymbolValueEoi, SymbolValueEoi));

  std::vector<StateNrType> pending;
  std::unordered_set<StateNrType> visited;

  auto const push_and_visit = [&pending, &visited](StateNrType state_nr_) {
    if (visited.contains(state_nr_)) {
      return;
    }

    pending.push_back(state_nr_);
    visited.insert(state_nr_);
  };

  push_and_visit(StateNrStart);

  while (!pending.empty()) {
    StateNrType const state_nr_cur = pending.back();
    pending.pop_back();

    State& state_cur = *_linecount_state_machine.get_state(state_nr_cur);

    if (state_cur.get_accepts().empty()) {
      alphabet.clone_asymmetric_difference(state_cur.get_symbol_values(SymbolKindCharacter)).for_each_interval([&](Interval<SymbolValueType> const& interval_) { state_cur.add_symbol_transition(SymbolKindCharacter, interval_, StateNrStart); });

      state_cur.get_symbol_values(SymbolKindCharacter).for_each_key([&](SymbolValueType symbol_) { push_and_visit(state_cur.get_symbol_transition(Symbol(SymbolKindCharacter, symbol_))); });
    }
  }
}

void LexerTableBuilder::fill_terminal_data() {
  for (GrammarData::TerminalAcceptData const& accept_data : _grammar_data->_terminal_accepts) {
    _result_tables.add_terminal_data(accept_data._label, accept_data._id_name);
  }
}

void LexerTableBuilder::validate_result() {
  IntervalSet<AcceptsIndexType> accepts_start = _lexer_state_machine.get_state(StateNrStart)->get_accepts();

  size_t const index_start = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_START, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  accepts_start.erase(Interval<AcceptsIndexType>(index_start));

  if (!accepts_start.empty()) {
    static size_t const MAX_REPORTED = 8;
    std::string msg;
    std::string delim;

    bool skip = false;
    size_t i = 0;
    accepts_start.for_each_key([&](AcceptsIndexType accept_nr_) {
      if (skip) {
        return;
      }

      msg += std::exchange(delim, ", ");
      if (i <= MAX_REPORTED) {
        msg += _grammar_data->_terminal_accepts[accept_nr_]._label;
        ++i;
      } else {
        skip = true;
        msg += "...";
      }
    });

    throw std::runtime_error("Error, initial state accepts terminal(s): " + msg);
  }

  size_t accepts_start_count = 0;
  IntervalSet<StateNrType> const state_nrs = _lexer_state_machine.get_state_nrs();
  for (size_t j = 0; j < state_nrs.size(); ++j) {
    Interval<StateNrType> const interval = state_nrs.get_by_index(j);
    for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *_lexer_state_machine.get_state(state_nr);
      if (state.get_accepts().size() == 0) {
        continue;
      }
      accepts_start_count += state.get_accepts().contains(index_start) ? 1 : 0;
    }
  }

  // Note: it actually seems to work with multiple starting states, but treat it as an error for now
  if (accepts_start_count != 1) {
    throw std::runtime_error("Error during building, there are " + std::to_string(accepts_start_count) + " starting states");
  }
}

void LexerTableBuilder::write_dot(std::string_view filename_, std::string_view title_, pmt::util::sm::ct::StateMachine const& state_machine_) const {
  static size_t const DOT_FILE_MAX_STATES = 750;
  if (state_machine_.get_state_count() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write of " << filename_ << " because it has " << state_machine_.get_state_count() << " states, which is more than the limit of " << DOT_FILE_MAX_STATES << '\n';
    return;
  }

  std::ofstream graph_file(filename_.data());
  std::ifstream skel_file("/home/pmt/repos/pmt/skel/pmt/util/smct/state_machine-skel.dot");

  GraphWriter::WriterArgs writer_args{._os_graph = graph_file, ._is_graph_skel = skel_file, ._state_machine = state_machine_};

  GraphWriter::StyleArgs style_args;
  style_args._title = title_;
  style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {
    return _result_tables.get_accept_index_label(accepts_);
  };

  GraphWriter().write_dot(writer_args, style_args);
  std::cout << "Wrote dot file: " << filename_ << '\n';
}

}  // namespace pmt::parser::builder
