#include "pmt/parserbuilder/parser_table_builder.hpp"

#include "pmt/base/bitset_converter.hpp"
#include "pmt/parserbuilder/grammar_data.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smct/state_machine_determinizer.hpp"
#include "pmt/util/smct/state_machine_minimizer.hpp"
#include "pmt/util/smct/state_machine_pruner.hpp"
#include "pmt/util/smct/graph_writer.hpp"
#include "pmt/util/smct/state.hpp"

#include <iostream>
#include <fstream>

namespace pmt::parserbuilder {
 using namespace pmt::base;
 using namespace pmt::util::smct;
 using namespace pmt::util::smrt;

namespace {
 auto get_symbol_kinds(State const& state_) -> IntervalSet<SymbolType> {
  IntervalSet<SymbolType> ret;
  state_.get_symbols().for_each_interval([&](Interval<SymbolType> interval_) {
   SymbolType const kind_lower = Symbol(interval_.get_lower()).get_kind();
   SymbolType const kind_upper = Symbol(interval_.get_upper()).get_kind();
   for (SymbolType kind_i = kind_lower; kind_i <= kind_upper; ++kind_i) {
    ret.insert(Interval<SymbolType>(kind_i));
   }
  });

  return ret;
 }
}

auto ParserTableBuilder::build(GenericAst const& ast_, GrammarData const& grammar_data_, LexerTables const& lexer_tables_) -> ParserTables {
 _ast = &ast_;
 _grammar_data = &grammar_data_;
 _lexer_tables = &lexer_tables_;

 setup_parser_state_machine();
 write_nonterminal_state_machine_dot("Initial tables", _result_tables._parser_state_machine);
 StateMachineDeterminizer::determinize(_result_tables._parser_state_machine);
 write_nonterminal_state_machine_dot("Determinized tables", _result_tables._parser_state_machine);
 StateMachineMinimizer::minimize(_result_tables._parser_state_machine);
 write_nonterminal_state_machine_dot("Minimized tables", _result_tables._parser_state_machine);
 extract_conflicts();
 write_nonterminal_state_machine_dot("Final tables", _result_tables._parser_state_machine);
 setup_lookahead_state_machine();
 StateMachineDeterminizer::determinize(_result_tables._lookahead_state_machine);
 StateMachineMinimizer::minimize(_result_tables._lookahead_state_machine);
 write_lookahead_state_machine_dot("Lookahead tables", _result_tables._lookahead_state_machine);

 fill_terminal_transition_masks();
 check_terminal_transition_masks();
 fill_conflict_transition_masks();
 fill_nonterminal_data();

 return std::move(_result_tables);
}

void ParserTableBuilder::setup_parser_state_machine() {
 StateNrType const state_nr_start = _result_tables._parser_state_machine.create_new_state();
 StateMachinePart state_machine_part_nonterminal = _nonterminal_state_machine_part_builder.build([this](size_t index_) { return lookup_nonterminal_label_by_index(index_); }, [this](std::string_view name_) { return lookup_nonterminal_index_by_label(name_); }, [this](size_t index_) { return lookup_nonterminal_definition_by_index(index_); }, [&](std::string_view label_){return *_lexer_tables->terminal_label_to_index(label_);}, *_ast, _grammar_data->_start_nonterminal_definition, _result_tables._parser_state_machine);
 StateNrType const state_nr_pre_end = _result_tables._parser_state_machine.create_new_state();
 State& state_pre_end = *_result_tables._parser_state_machine.get_state(state_nr_pre_end);
 StateNrType const state_nr_end = _result_tables._parser_state_machine.create_new_state();
 State& state_end = *_result_tables._parser_state_machine.get_state(state_nr_end);
 state_pre_end.add_symbol_transition(Symbol(SymbolKindTerminal, _lexer_tables->get_eoi_accept_index()), state_nr_end);
 state_end.get_accepts().resize(_grammar_data->_nonterminal_accepts.size(), false);
 _result_tables._eoi_accept_index = binary_find_index(_grammar_data->_nonterminal_accepts.begin(), _grammar_data->_nonterminal_accepts.end(), GrammarData::LABEL_EOI, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
 state_end.get_accepts().set(_result_tables._eoi_accept_index, true);
 state_machine_part_nonterminal.connect_outgoing_transitions_to(state_nr_pre_end, _result_tables._parser_state_machine);
 _result_tables._parser_state_machine.get_state(state_nr_start)->add_epsilon_transition(*state_machine_part_nonterminal.get_incoming_state_nr());
}

void ParserTableBuilder::extract_conflicts() {
 find_conflicting_state_nrs();

 size_t conflict_counter = 0;

 // extract the conflicts
 _conflicting_state_nrs.for_each_key([&](StateNrType state_nr_) {
  // create conflict transitions to the newly created states later
  IntervalSet<StateNrType> state_nrs_new;
  State& state_old = *_result_tables._parser_state_machine.get_state(state_nr_);
  // every accept needs to get a new state
  std::unordered_set<size_t> const accepts_old = BitsetConverter::to_unordered_set(state_old.get_accepts());
  for (size_t const accept : accepts_old) {
   StateNrType const state_nr_new = _result_tables._parser_state_machine.create_new_state();
   state_nrs_new.insert(Interval<StateNrType>(state_nr_new));
   State& state_new = *_result_tables._parser_state_machine.get_state(state_nr_new);
   state_new.get_accepts().resize(accept + 1, false);
   state_new.get_accepts().set(accept, true);
  }
  state_old.get_accepts().clear();

  // all terminal symbols need to be moved to another single new state
  IntervalSet<SymbolType> const terminal_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindTerminal, 0).get_combined(), Symbol(SymbolKindTerminal, SymbolValueMax).get_combined()));
  IntervalSet<SymbolType> const terminal_symbols = state_old.get_symbols().clone_and(terminal_symbol_mask);

  if (!terminal_symbols.empty()) {
   StateNrType const state_nr_new = _result_tables._parser_state_machine.create_new_state();
   state_nrs_new.insert(Interval<StateNrType>(state_nr_new));
   State& state_new = *_result_tables._parser_state_machine.get_state(state_nr_new);
   terminal_symbols.for_each_key([&](SymbolType symbol_) {
    StateNrType const state_nr_next = state_old.get_symbol_transition(Symbol(symbol_));
    state_new.add_symbol_transition(Symbol(symbol_), state_nr_next);
   });
  }

  // all open and close symbols need to be moved to another single new state
  IntervalSet<SymbolType> open_close_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindOpen, SymbolValueOpen).get_combined()));
  open_close_symbol_mask.insert(Interval<SymbolType>(Symbol(SymbolKindClose, 0).get_combined(), Symbol(SymbolKindClose, SymbolValueMax).get_combined()));
  IntervalSet<SymbolType> const open_close_symbols = state_old.get_symbols().clone_and(open_close_symbol_mask);
  
  if (!open_close_symbols.empty()) { 
   StateNrType const state_nr_new_open_close = _result_tables._parser_state_machine.create_new_state();
   state_nrs_new.insert(Interval<StateNrType>(state_nr_new_open_close));
   State& state_new_open_close = *_result_tables._parser_state_machine.get_state(state_nr_new_open_close);
   open_close_symbols.for_each_key([&](SymbolType symbol_) {
    StateNrType const state_nr_next = state_old.get_symbol_transition(Symbol(symbol_));
    state_new_open_close.add_symbol_transition(Symbol(symbol_), state_nr_next);
   });
  }

  // clear all symbols from the old state
  state_old.clear_symbol_transitions();

  // connect the old state to the new with conflict symbols
  state_nrs_new.for_each_key([&](StateNrType state_nr_new_) {
   state_old.add_symbol_transition(Symbol(SymbolKindConflict, conflict_counter++), state_nr_new_);
  });
 });
}

void ParserTableBuilder::setup_lookahead_state_machine() {
 if (_conflicting_state_nrs.empty()) {
  return;
 }

 std::unordered_map<SymbolType, std::unordered_set<StateNrType>> post_close_state_nrs;

 _result_tables._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_result_tables._parser_state_machine.get_state(state_nr_);
  IntervalSet<SymbolType> const close_mask (Interval<SymbolType>(Symbol(SymbolKindClose, 0).get_combined(), Symbol(SymbolKindClose, SymbolValueMax).get_combined()));
  IntervalSet<SymbolType> const close_symbols = state.get_symbols().clone_and(close_mask);
  close_symbols.for_each_key([&](SymbolType symbol_) {
   StateNrType const state_nr_next = state.get_symbol_transition(Symbol(symbol_));
   post_close_state_nrs[Symbol(symbol_).get_value()].insert(state_nr_next);
  });
 });

  _result_tables._lookahead_state_machine.get_or_create_state(StateNrStart);

  _conflicting_state_nrs.for_each_key([&](StateNrType state_nr_conflicting_) {
  State const& state_conflicting = *_result_tables._parser_state_machine.get_state(state_nr_conflicting_);

  StateMachine partial_conflict_state_machine;
  StateNrType const state_nr_partial_start = partial_conflict_state_machine.create_new_state();

  state_conflicting.get_symbols().for_each_key([&](SymbolType conflict_symbol_) {
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

   push_and_visit(_result_tables._parser_state_machine.get_state(state_nr_conflicting_)->get_symbol_transition(Symbol(conflict_symbol_)));

   while (!pending.empty()) {
    StateNrType const state_nr_cur_old = pending.back();
    pending.pop_back();
    StateNrType const state_nr_cur_new = *visited.find(state_nr_cur_old);
    State const& state_old = *_result_tables._parser_state_machine.get_state(state_nr_cur_old);
    State& state_new = state_machine_lookahead.get_or_create_state(state_nr_cur_new);

    state_new.get_accepts().resize(Symbol(conflict_symbol_).get_value() + 1, false);
    state_new.get_accepts().set(Symbol(conflict_symbol_).get_value(), true);

    assert(state_old.get_epsilon_transitions().empty());

    // SymbolKindTerminal
    {
     IntervalSet<SymbolType> const terminal_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindTerminal, 0).get_combined(), Symbol(SymbolKindTerminal, SymbolValueMax).get_combined()));
     IntervalSet<SymbolType> const terminal_symbols = state_old.get_symbols().clone_and(terminal_symbol_mask);

     terminal_symbols.for_each_key([&](SymbolType symbol_) {
      StateNrType const state_nr_next_old = state_old.get_symbol_transition(Symbol(symbol_));
      state_new.add_symbol_transition(Symbol(symbol_), push_and_visit(state_nr_next_old));
     });
    }

    // SymbolKindOpen
    {
     IntervalSet<SymbolType> const open_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindOpen, SymbolValueOpen).get_combined()));
     IntervalSet<SymbolType> const open_symbols = state_old.get_symbols().clone_and(open_symbol_mask);

     open_symbols.for_each_key([&](SymbolType symbol_) {
      StateNrType const state_nr_next_old = state_old.get_symbol_transition(Symbol(symbol_));
      state_new.add_epsilon_transition(push_and_visit(state_nr_next_old));
     });
    }

    // SymbolKindClose
    {
     std::unordered_set<size_t> const accepts_old = BitsetConverter::to_unordered_set(state_old.get_accepts());
     for (size_t const accept : accepts_old) {
      auto const itr = post_close_state_nrs.find(Symbol(SymbolKindClose, accept).get_value());
      if (itr != post_close_state_nrs.end()) {
       for (StateNrType const state_nr_next_old : itr->second) {
        StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
        state_new.add_epsilon_transition(state_nr_next_new);
       } 
      }
     }
    }

    // SymbolKindConflict
    {
     IntervalSet<SymbolType> const conflict_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindConflict, 0).get_combined(), Symbol(SymbolKindConflict, SymbolValueMax).get_combined()));
     IntervalSet<SymbolType> conflict_symbols = state_old.get_symbols().clone_and(conflict_symbol_mask);
    
     // remove the other conflict symbols from this conflict state
     IntervalSet<SymbolType> skip_conflict_symbols = state_conflicting.get_symbols();
     skip_conflict_symbols.erase(Interval<SymbolType>(conflict_symbol_));
     conflict_symbols.inplace_asymmetric_difference(skip_conflict_symbols);
     conflict_symbols.for_each_key([&](SymbolType symbol_) {
     StateNrType const state_nr_next_old = state_old.get_symbol_transition(Symbol(symbol_));
     state_new.add_epsilon_transition(push_and_visit(state_nr_next_old));
     });
    }
   }

   // merge this state machine into the partial conflict state machine
   StateNrType const state_nr_lookahead_start = partial_conflict_state_machine.get_unused_state_nr();
   StateMachinePruner::prune(state_machine_lookahead, StateNrStart, state_nr_lookahead_start);
   partial_conflict_state_machine.get_state(state_nr_partial_start)->add_epsilon_transition(state_nr_lookahead_start);

   state_machine_lookahead.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    partial_conflict_state_machine.get_or_create_state(state_nr_) = *state_machine_lookahead.get_state(state_nr_);
   });
  });

  StateMachineDeterminizer::determinize(partial_conflict_state_machine);
  try_solve_partial_conflict(partial_conflict_state_machine, state_conflicting.get_symbols());

  StateNrType const state_nr_prune_start = _result_tables._lookahead_state_machine.get_unused_state_nr();
  StateMachinePruner::prune(partial_conflict_state_machine, StateNrStart, state_nr_prune_start);
  _result_tables._lookahead_state_machine.get_state(StateNrStart)->add_epsilon_transition(state_nr_prune_start);
  partial_conflict_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
   _result_tables._lookahead_state_machine.get_or_create_state(state_nr_) = *partial_conflict_state_machine.get_state(state_nr_);
  });
 });
}

void ParserTableBuilder::fill_terminal_transition_masks() {
 // fill parser terminal transition masks
 _result_tables._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_result_tables._parser_state_machine.get_state(state_nr_);
  IntervalSet<SymbolType> const terminal_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindTerminal, 0).get_combined(), Symbol(SymbolKindTerminal, SymbolValueMax).get_combined()));
  IntervalSet<SymbolType> const terminal_symbols = state.get_symbols().clone_and(terminal_symbol_mask);
  if (terminal_symbols.empty()) {
   return;
  }
  Bitset mask(_lexer_tables->get_accept_count(), false);
  terminal_symbols.for_each_key([&](SymbolType symbol_) {
   mask.set(Symbol(symbol_).get_value(), true);
  });
  mask.set(Symbol(SymbolKindTerminal, _lexer_tables->get_start_accept_index()).get_value(), true);
  _result_tables._parser_terminal_transition_masks[state_nr_] = mask;
 });

 // fill lookahead terminal transition masks
 _result_tables._lookahead_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_result_tables._lookahead_state_machine.get_state(state_nr_);
  IntervalSet<SymbolType> const terminal_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindTerminal, 0).get_combined(), Symbol(SymbolKindTerminal, SymbolValueMax).get_combined()));
  IntervalSet<SymbolType> const terminal_symbols = state.get_symbols().clone_and(terminal_symbol_mask);
  if (terminal_symbols.empty()) {
   return;
  }
  Bitset mask(_lexer_tables->get_accept_count(), false);
  terminal_symbols.for_each_key([&](SymbolType symbol_) {
   mask.set(Symbol(symbol_).get_value(), true);
  });
  mask.set(Symbol(SymbolKindTerminal, _lexer_tables->get_start_accept_index()).get_value(), true);
  _result_tables._lookahead_terminal_transition_masks[state_nr_] = mask;
 });
}

void ParserTableBuilder::check_terminal_transition_masks() {
 // accept index -> other accept indices it can conflict with
 std::unordered_map<size_t, Bitset> terminal_accepts_masks;
 
 _lexer_tables->_lexer_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  Bitset const& accepts = _lexer_tables->_lexer_state_machine.get_state(state_nr_)->get_accepts();
  for (size_t i = 0; i < accepts.size(); ++i) {
   if (!accepts.get(i)) {
    continue;
   }
   auto itr_mask = terminal_accepts_masks.find(i);
   if (itr_mask == terminal_accepts_masks.end()) {
    itr_mask = terminal_accepts_masks.emplace(i, Bitset(_lexer_tables->get_accept_count(), false)).first;
   }

   itr_mask->second.inplace_or(accepts);
   itr_mask->second.set(i, false);
  }
 });

 // Check for conflicts in the parser state machine
 for (auto [state_nr, transitions] : _result_tables._parser_terminal_transition_masks) {
  for (size_t i = 0; i < transitions.size(); ++i) {
   if (!transitions.get(i) || i == _lexer_tables->get_start_accept_index()) {
    continue;
   }

   auto const itr = terminal_accepts_masks.find(i);
   
   if (itr == terminal_accepts_masks.end()) {
    continue;
   }

   transitions.set(i, false);
   
   if (transitions.clone_and(itr->second).any()) {
    std::string conflict_message = "Error, Terminal '" + _lexer_tables->get_accept_index_label(i) + "' conflicts with: ";
    std::string delim;
    for (size_t j = 0; j < itr->second.size(); ++j) {
     if (itr->second.get(j)) {
      conflict_message += std::exchange(delim, ", ") + "'" + _lexer_tables->get_accept_index_label(j) + "'";
     }
    }
    conflict_message += " in parser state " + std::to_string(state_nr) + "";
    throw std::runtime_error(conflict_message);
   }

   transitions.set(i, true);
  }
 }

 // Check for conflicts in the lookahead state machine
 for (auto [state_nr, transitions] : _result_tables._lookahead_terminal_transition_masks) {
  for (size_t i = 0; i < transitions.size(); ++i) {
   if (!transitions.get(i) || i == _lexer_tables->get_start_accept_index()) {
    continue;
   }

   auto const itr = terminal_accepts_masks.find(i);
   
   if (itr == terminal_accepts_masks.end()) {
    continue;
   }

   transitions.set(i, false);
   
   if (transitions.clone_and(itr->second).any()) {
    std::string conflict_message = "Error, Terminal '" + _lexer_tables->get_accept_index_label(i) + "' conflicts with: ";
    std::string delim;
    for (size_t j = 0; j < itr->second.size(); ++j) {
     if (itr->second.get(j)) {
      conflict_message += std::exchange(delim, ", ") + "'" + _lexer_tables->get_accept_index_label(j) + "'";
     }
    }
    conflict_message += " in lookahead state " + std::to_string(state_nr) + "";
    throw std::runtime_error(conflict_message);
   }

   transitions.set(i, true);
  }
 }
}

void ParserTableBuilder::fill_conflict_transition_masks() {
 _result_tables._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_result_tables._parser_state_machine.get_state(state_nr_);
  IntervalSet<SymbolType> const conflict_symbol_mask(Interval<SymbolType>(Symbol(SymbolKindConflict, 0).get_combined(), Symbol(SymbolKindConflict, SymbolValueMax).get_combined()));
  IntervalSet<SymbolType> const conflict_symbols = state.get_symbols().clone_and(conflict_symbol_mask);
  if (conflict_symbols.empty()) {
   return;
  }
  Bitset mask(_lexer_tables->get_accept_count(), false);
  conflict_symbols.for_each_key([&](SymbolType symbol_) {
   SymbolType const symbol_value = Symbol(symbol_).get_value();
   mask.set(symbol_value, true);
   _result_tables._conflict_count = std::max(_result_tables._conflict_count, symbol_value + 1);
  });
  _result_tables._parser_conflict_transition_masks[state_nr_] = mask;
 });
}

void ParserTableBuilder::fill_nonterminal_data() {
 // Associate ID strings with a numerical value
 std::unordered_map<std::string, size_t> string_to_id_map;
 _result_tables._min_id = _lexer_tables->get_min_id() + _lexer_tables->get_id_count();
 for (size_t i = 0; i < _grammar_data->_nonterminal_accepts.size(); ++i) {
   std::string const& id_name = _grammar_data->_nonterminal_accepts[i]._id_name;
   auto const itr = string_to_id_map.find(id_name);
   if (itr != string_to_id_map.end()) {
     continue;
   }
   string_to_id_map[id_name] = GenericId::is_generic_id(id_name) ? GenericId::string_to_id(id_name) : _result_tables._min_id + _result_tables._id_count++;
 }

 // Fill terminal data
 for (size_t i = 0; i < _grammar_data->_nonterminal_accepts.size(); ++i) {
  _result_tables._nonterminal_data.push_back(ParserTables::NonterminalData{
   ._label = _grammar_data->_nonterminal_accepts[i]._label,
   ._id = string_to_id_map.find(_grammar_data->_nonterminal_accepts[i]._id_name)->second,
   ._merge = _grammar_data->_nonterminal_accepts[i]._merge,
   ._unpack = _grammar_data->_nonterminal_accepts[i]._unpack,
   ._hide = _grammar_data->_nonterminal_accepts[i]._hide
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

void ParserTableBuilder::write_nonterminal_state_machine_dot(std::string title_, pmt::util::smct::StateMachine const& state_machine_) {
 std::string filename = "nonterminal_state_machine_" + std::to_string(_nonterminal_dotfile_counter++) + ".dot";
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

 GraphWriter::WriterArgs writer_args{
  ._os_graph = graph_file,
  ._is_graph_skel = skel_file,
  ._state_machine = state_machine_
 };

 GraphWriter::StyleArgs style_args;
 style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {
  return lookup_nonterminal_label_by_index(accepts_);
 };

 style_args._title = std::move(title_);
 style_args._symbols_to_label_fn = [&](IntervalSet<SymbolType> const& symbols_) -> std::string {
  std::string delim;
  std::string ret;
  symbols_.for_each_key([&](SymbolType symbol_) {
   Symbol symbol(symbol_);
   switch (symbol.get_kind()) {
    case SymbolKindTerminal:
     ret += std::exchange(delim, ", ") + _lexer_tables->get_accept_index_label(symbol.get_value());
     break;
    case SymbolKindOpen:
     ret += std::exchange(delim, ", ") + "+";
     break;
    case SymbolKindClose:
     ret += std::exchange(delim, ", ") + lookup_nonterminal_label_by_index(symbol.get_value());
     return;
    case SymbolKindConflict:
     ret += std::exchange(delim, ", ") + std::to_string(symbol.get_value());
     break;
    default:
     ret += std::exchange(delim, ", ") + "unknown";
     break;
   }
  });
  return ret;
 };
 style_args._symbol_kind_to_edge_style_fn = [&](SymbolType kind_) -> GraphWriter::EdgeStyle {
  switch (kind_) {
   case SymbolKindTerminal:
    return GraphWriter::EdgeStyle::Solid;
   case SymbolKindOpen:
    return GraphWriter::EdgeStyle::Solid;
   case SymbolKindClose:
    return GraphWriter::EdgeStyle::Dashed;
   default:
    return GraphWriter::EdgeStyle::Solid;
  }
 };

 style_args._symbol_kind_to_font_flags_fn = [&](SymbolType kind_) -> GraphWriter::FontFlags {
  switch (kind_) {
   case SymbolKindTerminal:
    return GraphWriter::FontFlags::Italic;
   case SymbolKindClose:
    return GraphWriter::FontFlags::Bold;
   default:
    return GraphWriter::FontFlags::None;
  }
 };

 style_args._symbol_kind_to_edge_color_fn = [&](SymbolType kind_) -> GraphWriter::Color {
  switch (kind_) {
   case SymbolKindConflict:
    return GraphWriter::Color{._r = 255, ._g = 165, ._b = 0};
   default:
    return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
  }
 };

 style_args._symbol_kind_to_font_color_fn = [&](SymbolType kind_) -> GraphWriter::Color {
  switch (kind_) {
   case SymbolKindConflict:
    return GraphWriter::Color{._r = 255, ._g = 165, ._b = 0};
   default:
    return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
  }
 };

 style_args._layout_direction = GraphWriter::LayoutDirection::TopToBottom;

 GraphWriter().write_dot(writer_args, style_args);
 std::cout << "Wrote dot file: " << filename << '\n';
}

void ParserTableBuilder::write_lookahead_state_machine_dot(std::string title_, pmt::util::smct::StateMachine const& state_machine_) {
 std::string filename = "lookahead_state_machine_" + std::to_string(_lookahead_dotfile_counter++) + ".dot";
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

 GraphWriter::WriterArgs writer_args{
  ._os_graph = graph_file,
  ._is_graph_skel = skel_file,
  ._state_machine = state_machine_
 };

 GraphWriter::StyleArgs style_args;
 style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {
  return std::to_string(accepts_);
 };

 style_args._title = std::move(title_);
 style_args._symbols_to_label_fn = [&](IntervalSet<SymbolType> const& symbols_) -> std::string {
  std::string delim;
  std::string ret;
  symbols_.for_each_key([&](SymbolType symbol_) {
   Symbol symbol(symbol_);
   switch (symbol.get_kind()) {
    case SymbolKindTerminal:
     ret += std::exchange(delim, ", ") + _lexer_tables->get_accept_index_label(symbol.get_value());
     break;
    default:
     ret += std::exchange(delim, ", ") + "unknown";
     break;
   }
  });
  return ret;
 };

 style_args._symbol_kind_to_font_flags_fn = [&](SymbolType kind_) -> GraphWriter::FontFlags {
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

auto ParserTableBuilder::lookup_nonterminal_label_by_index(size_t index_) const -> std::string {
 //assert(index_ < _grammar_data->_nonterminal_accepts.size());
 return (index_ < _grammar_data->_nonterminal_accepts.size()) ? _grammar_data->_nonterminal_accepts[index_]._label : "unknown";
}

auto ParserTableBuilder::lookup_nonterminal_index_by_label(std::string_view label_) const -> size_t {
 size_t const index = binary_find_index(_grammar_data->_nonterminal_accepts.begin(), _grammar_data->_nonterminal_accepts.end(), label_, [](auto const& lhs_, auto const& rhs_) { return GrammarData::FetchLabelString{}(lhs_) < GrammarData::FetchLabelString{}(rhs_); });
 assert(index != _grammar_data->_nonterminal_accepts.size());
 return index;
}

auto ParserTableBuilder::lookup_nonterminal_definition_by_index(size_t index_) const -> GenericAstPath {
 assert(index_ < _grammar_data->_nonterminal_accepts.size());
 return _grammar_data->_nonterminal_accepts[index_]._definition_path;
}

void ParserTableBuilder::find_conflicting_state_nrs()  {
 _result_tables._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_result_tables._parser_state_machine.get_state(state_nr_);
  IntervalSet<SymbolType> const symbol_kinds = get_symbol_kinds(state);
  
  // conflict if there is more than one accept or an accept with any symbol
  switch (state.get_accepts().popcnt()) {
   case 0:
    break;
   case 1:
    if (!symbol_kinds.empty()) {
     _conflicting_state_nrs.insert(Interval<StateNrType>(state_nr_));
    }
    return;
   default:
    _conflicting_state_nrs.insert(Interval<StateNrType>(state_nr_));
    return;
   }

   // conflict if an open + close state also has a terminal symbol
   if ((symbol_kinds.contains(SymbolKindClose) || symbol_kinds.contains(SymbolKindOpen)) && symbol_kinds.contains(SymbolKindTerminal)) {
    _conflicting_state_nrs.insert(Interval<StateNrType>(state_nr_));
   }
 });
}

void ParserTableBuilder::try_solve_partial_conflict(StateMachine& state_machine_, IntervalSet<SymbolType> conflict_symbols_) {
 std::unordered_map<SymbolType, std::unordered_set<StateNrType>> resolutions;

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

   if (state_cur.get_accepts().popcnt() == 1) { 
    resolutions[state_cur.get_accepts().countl(false)].insert(state_nr_cur);
    state_cur.clear_symbol_transitions();
    continue;
   } else {
    state_cur.get_accepts().clear();
   }

   state_cur.get_symbols().for_each_key([&](SymbolType symbol_) {
    push_and_visit(state_cur.get_symbol_transition(Symbol(symbol_)));
   });
 }

 StateMachinePruner::prune(state_machine_, StateNrStart);

 // check that all conflict symbols exist at least once
 IntervalSet<SymbolType> seen;
 state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *state_machine_.get_state(state_nr_);
  SymbolType const idx = state.get_accepts().countl(false);
  conflict_symbols_.erase(Interval<SymbolType>(Symbol(SymbolKindConflict, idx).get_combined()));
 });

 if (!conflict_symbols_.empty()) {
  throw std::runtime_error("Error during building, not all conflict symbols are resolved");
 }
}

}