#include "pmt/parserbuilder/parser_tables.hpp"

#include "pmt/asserts.hpp"
#include "pmt/util/smct/symbol.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smct;
using namespace pmt::util::smrt;

auto ParserTables::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 State const* state = _parser_state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(symbol_)) : StateNrSink;
}

auto ParserTables::get_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 auto const itr = _parser_terminal_transition_masks.find(state_nr_);
 return (itr != _parser_terminal_transition_masks.end()) ? itr->second.get_chunks() : pmt::base::Bitset::ChunkSpanConst{};
}

auto ParserTables::get_state_conflict_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 auto const itr = _parser_conflict_transition_masks.find(state_nr_);
 return (itr != _parser_conflict_transition_masks.end()) ? itr->second.get_chunks() : pmt::base::Bitset::ChunkSpanConst{};
}

auto ParserTables::get_state_kind(pmt::util::smrt::StateNrType state_nr_) const -> StateKind {
 State const* state = _parser_state_machine.get_state(state_nr_);
 assert(state);

 if (state->get_accepts().any()) {
  return StateTypeAccept;
 }
 
 if (state->get_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen)) != StateNrSink) {
  return StateTypeOpen;
 }

 IntervalSet<SymbolType> const symbols = state->get_symbols();
 IntervalSet<SymbolType> const mask_conflicts(Interval<SymbolType>(Symbol(SymbolKindConflict, 0).get_combined(), Symbol(SymbolKindConflict, SymbolValueMax).get_combined()));
 if (!symbols.clone_and(mask_conflicts).empty()) {
  return StateTypeConflict;
 }

 IntervalSet<SymbolType> const mask_terminals(Interval<SymbolType>(Symbol(SymbolKindTerminal, 0).get_combined(), Symbol(SymbolKindTerminal, SymbolValueMax).get_combined()));
 if (!symbols.clone_and(mask_terminals).empty()) {
  return StateTypeTerminal;
 }

 pmt::unreachable();
}

auto ParserTables::get_state_accept_index(pmt::util::smrt::StateNrType state_nr_) const -> size_t {
 State const* state = _parser_state_machine.get_state(state_nr_);
 if (!state) {
  return AcceptIndexInvalid;
 }

 size_t const countl = state->get_accepts().countl(false);
 return (countl == state->get_accepts().size()) ? AcceptIndexInvalid : countl;
}

auto ParserTables::get_accept_count() const -> size_t {
 return _nonterminal_data.size();
}

auto ParserTables::get_conflict_count() const -> size_t {
 return _conflict_count;
}

auto ParserTables::get_accept_index_label(size_t index_) const -> std::string {
 return _nonterminal_data[index_]._label;
}

auto ParserTables::get_accept_index_unpack(size_t index_) const -> bool {
 return _nonterminal_data[index_]._unpack;
}

auto ParserTables::get_accept_index_hide(size_t index_) const -> bool {
 return _nonterminal_data[index_]._hide;
}

auto ParserTables::get_accept_index_merge(size_t index_) const -> bool {
 return _nonterminal_data[index_]._merge;
}

auto ParserTables::get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType {
 return _nonterminal_data[index_]._id;
}

auto ParserTables::id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string {
 return _id_names[id_ - get_min_id()];
}

auto ParserTables::get_min_id() const -> pmt::util::smrt::GenericId::IdType {
 return _min_id;
}

auto ParserTables::get_id_count() const -> size_t {
 return _id_count;
}

auto ParserTables::get_lookahead_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::StateNrType symbol_) const -> pmt::util::smrt::StateNrType {
 State const* state = _lookahead_state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(symbol_)) : StateNrSink;
}

auto ParserTables::get_lookahead_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 auto const itr = _lookahead_terminal_transition_masks.find(state_nr_);
 return (itr != _lookahead_terminal_transition_masks.end()) ? itr->second.get_chunks() : pmt::base::Bitset::ChunkSpanConst{};
}


auto ParserTables::get_lookahead_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
 State const* state = _lookahead_state_machine.get_state(state_nr_);
 return state ? state->get_accepts().get_chunks() : pmt::base::Bitset::ChunkSpanConst{};
}

}