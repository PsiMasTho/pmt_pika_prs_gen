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

#include <map>

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;
using namespace pmt::util::smct;
using namespace pmt::base;

namespace {}

auto LexerTableBuilder::build(pmt::util::smrt::GenericAst const& ast_, GrammarData const& grammar_data_) -> LexerTables {
  _ast = &ast_;
  _grammar_data = &grammar_data_;
  setup_whitespace_state_machine();
  setup_terminal_state_machines();
  setup_comment_state_machines();
  loop_back_comment_close_state_machines();
  merge_comment_state_machines_into_result();
  merge_whitespace_state_machine_into_result();
  setup_start_and_eoi_states();
  connect_terminal_state_machines();
  StateMachineDeterminizer::determinize(_result_tables);
  StateMachineMinimizer::minimize(_result_tables);
  fill_terminal_data();
  validate_result();

  return std::move(_result_tables);
}

void LexerTableBuilder::setup_whitespace_state_machine() {
  if (_grammar_data->_whitespace_definition.empty()) {
    return;
  }

  size_t const index_whitespace = _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size();
  StateMachinePart fsm_part_whitespace = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_name_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_name(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, index_whitespace, _whitespace_state_machine);
  StateNrType const state_nr_end = _whitespace_state_machine.create_new_state();
  _whitespace_state_machine.get_state(state_nr_end)->get_accepts().resize(1, true);
  fsm_part_whitespace.connect_outgoing_transitions_to(state_nr_end, _whitespace_state_machine);
  StateMachineDeterminizer::determinize(_whitespace_state_machine);
}

void LexerTableBuilder::setup_terminal_state_machines() {
  for (size_t i = 0; i < _grammar_data->_terminals.size(); ++i) {
    if (!_grammar_data->_terminals[i]._accepts.has_value() || _grammar_data->_terminals[i]._name == GrammarData::TERMINAL_NAME_EOI || _grammar_data->_terminals[i]._name == GrammarData::TERMINAL_NAME_START) {
      continue;
    }

    StateMachine state_machine_terminal;
    StateMachinePart state_machine_part_terminal = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_name_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_name(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, i, state_machine_terminal);
    StateNrType const state_nr_end = state_machine_terminal.create_new_state();
    State& state_end = *state_machine_terminal.get_state(state_nr_end);
    state_end.get_accepts().resize(_grammar_data->_terminals.size(), false);
    state_end.get_accepts().set(*_grammar_data->_terminals[i]._accepts, true);
    state_machine_part_terminal.connect_outgoing_transitions_to(state_nr_end, state_machine_terminal);
    StateMachineDeterminizer::determinize(state_machine_terminal);
    _terminal_state_machines.push_back(std::move(state_machine_terminal));
  }
}

void LexerTableBuilder::setup_comment_state_machines() {
  for (size_t i = 0; i < _grammar_data->_comment_open_definitions.size(); ++i) {
   size_t const index_comment_open = _grammar_data->_terminals.size() + i;

   StateMachine state_machine_comment_open;
    StateMachinePart state_machine_part_rule_open = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_name_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_name(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, index_comment_open, state_machine_comment_open);
    StateNrType const state_nr_end = state_machine_comment_open.create_new_state();
    state_machine_comment_open.get_state(state_nr_end)->get_accepts().resize(1, true);
    state_machine_part_rule_open.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_open);
    StateMachineDeterminizer::determinize(state_machine_comment_open);
    _comment_open_state_machines.push_back(std::move(state_machine_comment_open));
  }

  for (size_t i = 0; i < _grammar_data->_comment_close_definitions.size(); ++i) {
    size_t const index_comment_close = _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size() + i;

    StateMachine state_machine_comment_close;
    StateMachinePart state_machine_part_rule_close = _terminal_state_machine_part_builder.build([this](size_t index_) { return lookup_terminal_name_by_index(index_); }, [this](std::string_view name_) { return lookup_terminal_index_by_name(name_); }, [this](size_t index_) { return lookup_terminal_definition_by_index(index_); }, *_ast, index_comment_close, state_machine_comment_close);
    StateNrType const state_nr_end = state_machine_comment_close.create_new_state();
    state_machine_comment_close.get_state(state_nr_end)->get_accepts().resize(1, true);
    state_machine_part_rule_close.connect_outgoing_transitions_to(state_nr_end, state_machine_comment_close);
    StateMachineDeterminizer::determinize(state_machine_comment_close);
    _comment_close_state_machines.push_back(std::move(state_machine_comment_close));
  }
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
        for (SymbolType symbol = 0; symbol < UCHAR_MAX; ++symbol) {
          StateNrType const state_nr_next = state_cur.get_symbol_transition(Symbol(symbol));
          if (state_nr_next != StateNrSink) {
            push_and_visit(state_nr_next);
          } else {
            state_cur.add_symbol_transition(Symbol(symbol), StateNrStart);
          }
        }
      }
    }
  }
}

void LexerTableBuilder::merge_comment_state_machines_into_result() {
  _result_tables.get_or_create_state(StateNrStart);

  for (size_t i = 0; i < _comment_open_state_machines.size(); ++i) {
    StateMachine& state_machine_comment_open = _comment_open_state_machines[i];
    StateNrType const state_nr_comment_open = _result_tables.create_new_state();
    StateMachinePruner::prune(state_machine_comment_open, StateNrStart, state_nr_comment_open, true);

    StateMachine& state_machine_comment_close = _comment_close_state_machines[i];
    StateNrType const state_nr_comment_close = state_nr_comment_open + state_machine_comment_close.get_state_count();
    StateMachinePruner::prune(state_machine_comment_close, StateNrStart, state_nr_comment_close, true);

    _result_tables.get_state(StateNrStart)->add_epsilon_transition(state_nr_comment_open);

    state_machine_comment_open.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
      State& state = *state_machine_comment_open.get_state(state_nr_);

      if (state.get_accepts().popcnt() != 0) {
        state.get_accepts().clear();
        state.add_epsilon_transition(state_nr_comment_close);
      }

      _result_tables.get_or_create_state(state_nr_) = state;
    });

    state_machine_comment_close.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
      State& state = *state_machine_comment_close.get_state(state_nr_);

      if (state.get_accepts().popcnt() != 0) {
        state.get_accepts().clear();
        state.add_epsilon_transition(StateNrStart);
      }

      _result_tables.get_or_create_state(state_nr_) = state;
    });
  }
}

void LexerTableBuilder::merge_whitespace_state_machine_into_result() {
 StateNrType const state_nr_whitespace = _result_tables.create_new_state();
 StateMachinePruner::prune(_whitespace_state_machine, StateNrStart, state_nr_whitespace, true);

 _result_tables.get_state(StateNrStart)->add_epsilon_transition(state_nr_whitespace);
 _whitespace_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
   State& state = *_whitespace_state_machine.get_state(state_nr_);
   if (state.get_accepts().popcnt() != 0) {
     state.get_accepts().clear();
     state.add_epsilon_transition(StateNrStart);
   }
   _result_tables.get_or_create_state(state_nr_) = state;
 });
}

void LexerTableBuilder::setup_start_and_eoi_states() {
  State& state_start = _result_tables.get_or_create_state(StateNrStart);
  state_start.get_accepts().resize(_grammar_data->_terminals.size(), false);
  size_t const index_start =  binary_find_index(_grammar_data->_terminals.begin(), _grammar_data->_terminals.end(), GrammarData::TERMINAL_NAME_START, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchNameString{}(lhs_) < GrammarData::FetchNameString{}(rhs_); });
  state_start.get_accepts().set(*_grammar_data->_terminals[index_start]._accepts, true);
  _result_tables._start_accept_index = *_grammar_data->_terminals[index_start]._accepts;

  StateNrType const state_nr_eoi = _result_tables.create_new_state();
  State& state_eoi = *_result_tables.get_state(state_nr_eoi);
  state_eoi.get_accepts().resize(_grammar_data->_terminals.size(), false);
  size_t const index_eoi = binary_find_index(_grammar_data->_terminals.begin(), _grammar_data->_terminals.end(), GrammarData::TERMINAL_NAME_EOI, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchNameString{}(lhs_) < GrammarData::FetchNameString{}(rhs_); });
  state_eoi.get_accepts().set(*_grammar_data->_terminals[index_eoi]._accepts, true);
  _result_tables._eoi_accept_index = *_grammar_data->_terminals[index_eoi]._accepts;

  state_start.add_symbol_transition(Symbol(SymbolEoi), state_nr_eoi);
}

void LexerTableBuilder::connect_terminal_state_machines() {
  State& state_start = *_result_tables.get_state(StateNrStart);

  for (size_t i = 0; i < _terminal_state_machines.size(); ++i) {
    StateMachine& state_machine_terminal = _terminal_state_machines[i];
    StateNrType const state_nr_terminal = _result_tables.get_unused_state_nr();
    StateMachinePruner::prune(state_machine_terminal, StateNrStart, state_nr_terminal, true);

    state_machine_terminal.get_state_nrs().for_each_key([&](StateNrType state_nr_) { _result_tables.get_or_create_state(state_nr_) = *state_machine_terminal.get_state(state_nr_); });

    state_start.add_epsilon_transition(state_nr_terminal);
  }
}

void LexerTableBuilder::fill_terminal_data() {
 // Associate ID strings with a numerical value
 std::unordered_map<std::string, size_t> string_to_id_map;
 for (GenericId::IdType counter = 0; size_t const rev : _grammar_data->_accepts_reverse) {
   std::string const& id_name = _grammar_data->_terminals[rev]._id_name;
   auto const itr = string_to_id_map.find(id_name);
   if (itr != string_to_id_map.end()) {
     continue;
   }
   string_to_id_map[id_name] = GenericId::is_generic_id(id_name) ? GenericId::string_to_id(id_name) : counter++;
 }

 // Fill terminal data
 for (size_t const rev : _grammar_data->_accepts_reverse) {
  _result_tables._terminal_data.push_back(LexerTables::TerminalData{
   ._name = _grammar_data->_terminals[rev]._name,
   ._id = string_to_id_map.find(_grammar_data->_terminals[rev]._id_name)->second
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
  Bitset accepts_start = _result_tables.get_state(StateNrStart)->get_accepts();

  size_t const index_start = _grammar_data->_terminals[binary_find_index(_grammar_data->_terminals.begin(), _grammar_data->_terminals.end(), GrammarData::TERMINAL_NAME_START, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchNameString{}(lhs_) < GrammarData::FetchNameString{}(rhs_); })]._accepts.value();
  accepts_start.set(index_start, false);

  if (accepts_start.any()) {
    static size_t const MAX_REPORTED = 8;
    std::unordered_set<size_t> const accepts_start_set = pmt::base::BitsetConverter::to_unordered_set(accepts_start);
    std::string msg;
    std::string delim;
    for (size_t i = 1; size_t const accept_nr : accepts_start_set) {
      msg += std::exchange(delim, ", ");
      if (i <= MAX_REPORTED) {
        msg += _grammar_data->_terminals[_grammar_data->_accepts_reverse[accept_nr]]._name;
      } else {
        msg += "...";
        break;
      }
    }
    throw std::runtime_error("Initial state accepts terminal(s): " + msg);
  }

  size_t accepts_start_count = 0;
  IntervalSet<StateNrType> const state_nrs = _result_tables.get_state_nrs();
  for (size_t j = 0; j < state_nrs.size(); ++j) {
    Interval<StateNrType> const interval = state_nrs.get_by_index(j);
    for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *_result_tables.get_state(state_nr);
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

auto LexerTableBuilder::lookup_terminal_name_by_index(size_t index_) const -> std::optional<std::string> {
 if (index_ >= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size() + 1) {
  return std::nullopt;
 }
 if (index_ >= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size()) {
  return GrammarData::TERMINAL_NAME_WHITESPACE;
 }
 if (index_ >= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size()) {
  index_ -= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size();
  return GrammarData::TERMINAL_COMMENT_CLOSE_PREFIX + std::to_string(index_);
 }
 if (index_ >= _grammar_data->_terminals.size()) {
  index_ -= _grammar_data->_terminals.size();
  return GrammarData::TERMINAL_COMMENT_OPEN_PREFIX + std::to_string(index_);
 }

 return _grammar_data->_terminals[index_]._name;
}

auto LexerTableBuilder::lookup_terminal_index_by_name(std::string_view name_) const -> std::optional<size_t> {
  assert(std::is_sorted(_grammar_data->_terminals.begin(), _grammar_data->_terminals.end(), [](auto const& lhs_, auto const& rhs_) { return lhs_._name < rhs_._name; }));

  if (name_.starts_with(GrammarData::TERMINAL_RESERVED_PREFIX_CH)) {
    return TerminalIndexAnonymous;
  }

  auto const it = std::lower_bound(_grammar_data->_terminals.begin(), _grammar_data->_terminals.end(), name_, [](auto const& lhs_, auto const& rhs_) { return lhs_._name < rhs_; });

  if (it == _grammar_data->_terminals.end() || it->_name != name_) {
    return std::nullopt;
  }

  return std::distance(_grammar_data->_terminals.begin(), it);
}

auto LexerTableBuilder::lookup_terminal_definition_by_index(size_t index_) const -> std::optional<pmt::util::smrt::GenericAstPath> {
 if (index_ >= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size() + 1) {
  return std::nullopt;
 }
 if (index_ >= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size() + _grammar_data->_comment_close_definitions.size()) {
  return _grammar_data->_whitespace_definition;
 }
 if (index_ >= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size()) {
  index_ -= _grammar_data->_terminals.size() + _grammar_data->_comment_open_definitions.size();
  return _grammar_data->_comment_close_definitions[index_];
 }
 if (index_ >= _grammar_data->_terminals.size()) {
  index_ -= _grammar_data->_terminals.size();
  return _grammar_data->_comment_open_definitions[index_];
 }

 return _grammar_data->_terminals[index_]._definition_path;
}

}  // namespace pmt::parserbuilder
