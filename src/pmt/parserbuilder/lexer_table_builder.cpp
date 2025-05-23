#include "pmt/parserbuilder/lexer_table_builder.hpp"

#include "pmt/base/algo.hpp"
#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/state_machine_determinizer.hpp"
#include "pmt/util/smct/state_machine_minimizer.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smct/state_machine_pruner.hpp"
#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smct/graph_writer.hpp"
#include "pmt/parserbuilder/grammar_data.hpp"


#include <iostream>
#include <fstream>
#include <map>

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;
using namespace pmt::util::smct;
using namespace pmt::base;

namespace {
}

auto LexerTableBuilder::build(pmt::util::smrt::GenericAst const& ast_, GrammarData const& grammar_data_) -> LexerTables {
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
  _result_tables._lexer_state_machine = StateMachineDeterminizer(_result_tables._lexer_state_machine).determinize();
  StateMachineMinimizer::minimize(_result_tables._lexer_state_machine);
  write_dot("linecount_state_machine.dot", _result_tables._linecount_state_machine);
  StateMachineMinimizer::minimize(_result_tables._linecount_state_machine);
  write_dot("lexer_tables.dot", _result_tables._lexer_state_machine);
  validate_result();

  return std::move(_result_tables);
}

void LexerTableBuilder::setup_whitespace_state_machine() {
  if (_grammar_data->_whitespace_definition.empty()) {
    return;
  }

  size_t const index_whitespace = _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size();
  StateMachinePart fsm_part_whitespace = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_label(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, index_whitespace, _whitespace_state_machine);
  StateNrType const state_nr_end = _whitespace_state_machine.create_new_state();
  _whitespace_state_machine.get_state(state_nr_end)->get_accepts().resize(1, true);
  fsm_part_whitespace.connect_outgoing_transitions_to(state_nr_end, _whitespace_state_machine);
  _whitespace_state_machine = StateMachineDeterminizer(_whitespace_state_machine).determinize();
}

void LexerTableBuilder::setup_terminal_state_machines() {
  for (size_t i = 0; i < _grammar_data->_terminal_accepts.size(); ++i) {
    if (!_grammar_data->_terminal_accepts[i]._accepted || _grammar_data->_terminal_accepts[i]._label == GrammarData::LABEL_EOI || _grammar_data->_terminal_accepts[i]._label == GrammarData::TERMINAL_LABEL_START || _grammar_data->_terminal_accepts[i]._label == GrammarData::TERMINAL_LABEL_LINECOUNT) {
      continue;
    }

    StateMachine state_machine_terminal;
    StateMachinePart state_machine_part_terminal = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_label(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, i, state_machine_terminal);
    StateNrType const state_nr_end = state_machine_terminal.create_new_state();
    State& state_end = *state_machine_terminal.get_state(state_nr_end);
    state_end.get_accepts().resize(_grammar_data->_terminal_accepts.size(), false);
    state_end.get_accepts().set(i, true);
    state_machine_part_terminal.connect_outgoing_transitions_to(state_nr_end, state_machine_terminal);
    state_machine_terminal = StateMachineDeterminizer(state_machine_terminal).determinize();
    _terminal_state_machines.push_back(std::move(state_machine_terminal));
  }
}

void LexerTableBuilder::setup_comment_state_machines() {
  for (size_t i = 0; i < _grammar_data->_comment_open_definitions.size(); ++i) {
   size_t const index_comment_open = _grammar_data->_terminal_accepts.size() + i;

    StateMachine state_machine_comment_open;
    StateMachinePart state_machine_part_comment_open = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_label(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, index_comment_open, state_machine_comment_open);
    StateNrType const state_nr_end = state_machine_comment_open.create_new_state();
    state_machine_comment_open.get_state(state_nr_end)->get_accepts().resize(1, true);
    state_machine_part_comment_open.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_open);
    state_machine_comment_open = StateMachineDeterminizer(state_machine_comment_open).determinize();
    _comment_open_state_machines.push_back(std::move(state_machine_comment_open));
  }

  for (size_t i = 0; i < _grammar_data->_comment_close_definitions.size(); ++i) {
    size_t const index_comment_close = _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + i;

    StateMachine state_machine_comment_close;
    StateMachinePart state_machine_part_comment_close = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_label(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, index_comment_close, state_machine_comment_close);
    StateNrType const state_nr_end = state_machine_comment_close.create_new_state();
    state_machine_comment_close.get_state(state_nr_end)->get_accepts().resize(1, true);
    state_machine_part_comment_close.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_close);
    state_machine_comment_close = StateMachineDeterminizer(state_machine_comment_close).determinize();
    _comment_close_state_machines.push_back(std::move(state_machine_comment_close));
  }
}

void LexerTableBuilder::setup_linecount_state_machine() {
 if (_grammar_data->_linecount_definition.empty()) {
  setup_default_linecount_state_machine();
  return;
 }

 size_t const index_linecount = _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size() + 1;
 StateMachinePart fsm_part_linecount = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_label_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_label(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, index_linecount, _result_tables._linecount_state_machine);
 StateNrType const state_nr_end = _result_tables._linecount_state_machine.create_new_state();
 State& state_end = *_result_tables._linecount_state_machine.get_state(state_nr_end);
 fsm_part_linecount.connect_outgoing_transitions_to(state_nr_end, _result_tables._linecount_state_machine);
 state_end.get_accepts().resize(_grammar_data->_terminal_accepts.size(), false);
 size_t const index_linecount_terminal = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_LINECOUNT, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
 state_end.get_accepts().set(index_linecount_terminal, true);
 _result_tables._linecount_state_machine = StateMachineDeterminizer(_result_tables._linecount_state_machine).determinize();
}

void LexerTableBuilder::setup_default_linecount_state_machine() {
 State& state_start = _result_tables._linecount_state_machine.get_or_create_state(StateNrStart);
 StateNrType const state_nr_end = _result_tables._linecount_state_machine.create_new_state();
 State& state_end = *_result_tables._linecount_state_machine.get_state(state_nr_end);
 state_end.get_accepts().resize(_grammar_data->_terminal_accepts.size(), false);
 size_t const index_linecount_terminal = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_LINECOUNT, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
 state_end.get_accepts().resize(_grammar_data->_terminal_accepts.size(), false);
 state_end.get_accepts().set(index_linecount_terminal, true);
 state_start.add_symbol_transition(Symbol('\n'), state_nr_end);
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

      if (state_cur.get_accepts().popcnt() == 0) {
        IntervalSet<SymbolType> const alphabet(Interval<SymbolType>(0, UCHAR_MAX));
        alphabet.clone_asymmetric_difference(state_cur.get_symbols()).for_each_interval([&](Interval<SymbolType> const& interval_) {
          state_cur.add_symbol_transition(interval_, StateNrStart);
        });

        state_cur.get_symbols().for_each_key([&](SymbolType symbol_) {
          push_and_visit(state_cur.get_symbol_transition(Symbol(symbol_)));
        });
      }
    }
  }
}

void LexerTableBuilder::merge_comment_state_machines_into_result() {
  _result_tables._lexer_state_machine.get_or_create_state(StateNrStart);

  for (size_t i = 0; i < _comment_open_state_machines.size(); ++i) {
    StateMachine& state_machine_comment_open = _comment_open_state_machines[i];
    StateNrType const state_nr_comment_open = _result_tables._lexer_state_machine.create_new_state();
    state_machine_comment_open = StateMachinePruner(state_machine_comment_open).enable_renumbering(state_nr_comment_open).prune();

    StateMachine& state_machine_comment_close = _comment_close_state_machines[i];
    StateNrType const state_nr_comment_close = state_nr_comment_open + state_machine_comment_close.get_state_count();
    state_machine_comment_close = StateMachinePruner(state_machine_comment_close).enable_renumbering(state_nr_comment_close).prune();

    _result_tables._lexer_state_machine.get_state(StateNrStart)->add_epsilon_transition(state_nr_comment_open);

    state_machine_comment_open.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
      State& state = *state_machine_comment_open.get_state(state_nr_);

      if (state.get_accepts().popcnt() != 0) {
        state.get_accepts().clear();
        state.add_epsilon_transition(state_nr_comment_close);
      }

      _result_tables._lexer_state_machine.get_or_create_state(state_nr_) = state;
    });

    state_machine_comment_close.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
      State& state = *state_machine_comment_close.get_state(state_nr_);

      if (state.get_accepts().popcnt() != 0) {
        state.get_accepts().clear();
        state.add_epsilon_transition(StateNrStart);
      }

      _result_tables._lexer_state_machine.get_or_create_state(state_nr_) = state;
    });
  }
}

void LexerTableBuilder::merge_whitespace_state_machine_into_result() {
 if (_grammar_data->_whitespace_definition.empty()) {
   return;
 }

 StateNrType const state_nr_whitespace = _result_tables._lexer_state_machine.create_new_state();
 _whitespace_state_machine = StateMachinePruner(_whitespace_state_machine).enable_renumbering(state_nr_whitespace).prune();
 _result_tables._lexer_state_machine.get_state(StateNrStart)->add_epsilon_transition(state_nr_whitespace);
 _whitespace_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
   State& state = *_whitespace_state_machine.get_state(state_nr_);
   if (state.get_accepts().popcnt() != 0) {
     state.get_accepts().clear();
     state.add_epsilon_transition(StateNrStart);
   }
   _result_tables._lexer_state_machine.get_or_create_state(state_nr_) = state;
 });
}

void LexerTableBuilder::setup_start_and_eoi_states() {
  State& state_start = _result_tables._lexer_state_machine.get_or_create_state(StateNrStart);
  state_start.get_accepts().resize(_grammar_data->_terminal_accepts.size(), false);
  size_t const index_start =  binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_START, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  state_start.get_accepts().set(index_start, true);
  _result_tables._start_accept_index = index_start;

  StateNrType const state_nr_eoi = _result_tables._lexer_state_machine.create_new_state();
  State& state_eoi = *_result_tables._lexer_state_machine.get_state(state_nr_eoi);
  state_eoi.get_accepts().resize(_grammar_data->_terminal_accepts.size(), false);
  size_t const index_eoi = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::LABEL_EOI, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  state_eoi.get_accepts().set(index_eoi, true);
  _result_tables._eoi_accept_index = index_eoi;

  state_start.add_symbol_transition(Symbol(SymbolValueEoi), state_nr_eoi);
}

void LexerTableBuilder::connect_terminal_state_machines() {
  State& state_start = *_result_tables._lexer_state_machine.get_state(StateNrStart);

  for (size_t i = 0; i < _terminal_state_machines.size(); ++i) {
    StateMachine& state_machine_terminal = _terminal_state_machines[i];
    StateNrType const state_nr_terminal = _result_tables._lexer_state_machine.get_unused_state_nr();
    state_machine_terminal = StateMachinePruner(state_machine_terminal).enable_renumbering(state_nr_terminal).prune();

    state_machine_terminal.get_state_nrs().for_each_key([&](StateNrType state_nr_) { _result_tables._lexer_state_machine.get_or_create_state(state_nr_) = *state_machine_terminal.get_state(state_nr_); });

    state_start.add_epsilon_transition(state_nr_terminal);
  }
}

void LexerTableBuilder::loop_back_linecount_state_machine() {
 IntervalSet<SymbolType> alphabet(Interval<SymbolType>(0, UCHAR_MAX));
 alphabet.insert(Interval<SymbolType>(SymbolValueEoi, SymbolValueEoi));

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

   State& state_cur = *_result_tables._linecount_state_machine.get_state(state_nr_cur);

   if (state_cur.get_accepts().popcnt() == 0) {
     alphabet.clone_asymmetric_difference(state_cur.get_symbols()).for_each_interval([&](Interval<SymbolType> const& interval_) {
       state_cur.add_symbol_transition(interval_, StateNrStart);
     });

     state_cur.get_symbols().for_each_key([&](SymbolType symbol_) {
       push_and_visit(state_cur.get_symbol_transition(Symbol(symbol_)));
     });
   }
 }
}

void LexerTableBuilder::fill_terminal_data() {
 // Associate ID strings with a numerical value
 std::unordered_map<std::string, size_t> string_to_id_map;
 for (size_t i = 0; i < _grammar_data->_terminal_accepts.size(); ++i) {
   std::string const& id_name = _grammar_data->_terminal_accepts[i]._id_name;
   auto const itr = string_to_id_map.find(id_name);
   if (itr != string_to_id_map.end()) {
     continue;
   }
   string_to_id_map[id_name] = GenericId::is_generic_id(id_name) ? GenericId::string_to_id(id_name) : _result_tables._id_count++;
 }

 // Fill terminal data
 for (size_t i = 0; i < _grammar_data->_terminal_accepts.size(); ++i) {
  _result_tables._terminal_data.push_back(LexerTables::TerminalData{
   ._label = _grammar_data->_terminal_accepts[i]._label,
   ._id = string_to_id_map.find(_grammar_data->_terminal_accepts[i]._id_name)->second,
   ._hide = _grammar_data->_terminal_accepts[i]._hide,
  });
 }

 // Fill ID to string mapping
 std::map<size_t, std::string> id_to_string_map;
 for (auto const& [name, id] : string_to_id_map) {
   if (GenericId::is_generic_id(id)) {
     continue;
   }
   id_to_string_map[id] = name;
 }

 for (auto const& [id, string] : id_to_string_map) {
   _result_tables._id_names.push_back(string);
 }
}

void LexerTableBuilder::validate_result() {
  Bitset accepts_start = _result_tables._lexer_state_machine.get_state(StateNrStart)->get_accepts();

  size_t const index_start = binary_find_index(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), GrammarData::TERMINAL_LABEL_START, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
  accepts_start.set(index_start, false);

  if (accepts_start.any()) {
    static size_t const MAX_REPORTED = 8;
    std::unordered_set<size_t> const accepts_start_set = pmt::base::BitsetConverter::to_unordered_set(accepts_start);
    std::string msg;
    std::string delim;
    for (size_t i = 1; size_t const accept_nr : accepts_start_set) {
      msg += std::exchange(delim, ", ");
      if (i <= MAX_REPORTED) {
        msg += _grammar_data->_terminal_accepts[accept_nr]._label;
      } else {
        msg += "...";
        break;
      }
    }
    throw std::runtime_error("Initial state accepts terminal(s): " + msg);
  }

  size_t accepts_start_count = 0;
  IntervalSet<StateNrType> const state_nrs = _result_tables._lexer_state_machine.get_state_nrs();
  for (size_t j = 0; j < state_nrs.size(); ++j) {
    Interval<StateNrType> const interval = state_nrs.get_by_index(j);
    for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *_result_tables._lexer_state_machine.get_state(state_nr);
      if (state.get_accepts().size() == 0) {
        continue;
      }
      accepts_start_count += state.get_accepts().get(index_start) == true ? 1 : 0;
    }
  }

  // Note: it actually seems to work with multiple starting states, but treat it as an error for now
  if (accepts_start_count != 1) {
    throw std::runtime_error("Error during building, there are " + std::to_string(accepts_start_count) + " starting states");
  }
}

void LexerTableBuilder::write_dot(std::string_view filename_, pmt::util::smct::StateMachine const& state_machine_) const {
 static size_t const DOT_FILE_MAX_STATES = 750;
 if (state_machine_.get_state_count() > DOT_FILE_MAX_STATES) {
  std::cerr << "Skipping dot file write of " << filename_ << " because it has " << state_machine_.get_state_count() << " states, which is more than the limit of " << DOT_FILE_MAX_STATES << '\n';
  return;
 }

 std::ofstream graph_file(filename_.data());
 std::ifstream skel_file("/home/pmt/repos/pmt/skel/pmt/util/smct/state_machine-skel.dot");

 GraphWriter::WriterArgs writer_args{
  ._os_graph = graph_file,
  ._is_graph_skel = skel_file,
  ._state_machine = state_machine_
 };

 GraphWriter::StyleArgs style_args;
 style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {return _result_tables.get_accept_index_label(accepts_);};

 GraphWriter().write_dot(writer_args, style_args);
 std::cout << "Wrote dot file: " << filename_ << '\n';
}

auto LexerTableBuilder::lookup_terminal_label_by_index(size_t index_) const -> std::string {
 assert(index_ < _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size() + 1);

 if (index_ >= _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size()) {
  return GrammarData::TERMINAL_LABEL_WHITESPACE;
 }
 if (index_ >= _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size()) {
  index_ -= _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size();
  return GrammarData::TERMINAL_COMMENT_CLOSE_PREFIX + std::to_string(index_);
 }
 if (index_ >= _grammar_data->_terminal_accepts.size()) {
  index_ -= _grammar_data->_terminal_accepts.size();
  return GrammarData::TERMINAL_COMMENT_OPEN_PREFIX + std::to_string(index_);
 }

 return _grammar_data->_terminal_accepts[index_]._label;
}

auto LexerTableBuilder::lookup_terminal_index_by_label(std::string_view label_) const -> size_t {
  assert(std::is_sorted(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), [](auto const& lhs_, auto const& rhs_) { return lhs_._label < rhs_._label; }));

  if (label_.starts_with(GrammarData::TERMINAL_RESERVED_PREFIX_CH)) {
    return TerminalIndexAnonymous;
  }

  auto const it = std::lower_bound(_grammar_data->_terminal_accepts.begin(), _grammar_data->_terminal_accepts.end(), label_, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });

  assert(it != _grammar_data->_terminal_accepts.end() && it->_label == label_);

  return std::distance(_grammar_data->_terminal_accepts.begin(), it);
}

auto LexerTableBuilder::lookup_terminal_definition_by_index(size_t index_) const -> pmt::util::smrt::GenericAstPath {
 assert(index_ < _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size() + 2);

 if (index_ >= _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size() + 1) {
  return _grammar_data->_linecount_definition;
 }
 if (index_ >= _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size()) {
  return _grammar_data->_whitespace_definition;
 }
 if (index_ >= _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size()) {
  index_ -= _grammar_data->_terminal_accepts.size() + _grammar_data->_comment_open_definitions.size();
  return _grammar_data->_comment_close_definitions[index_];
 }
 if (index_ >= _grammar_data->_terminal_accepts.size()) {
  index_ -= _grammar_data->_terminal_accepts.size();
  return _grammar_data->_comment_open_definitions[index_];
 }

 return _grammar_data->_terminal_accepts[index_]._definition_path;
}

}  // namespace pmt::parserbuilder
