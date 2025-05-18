#include "pmt/parserbuilder/lexer_tables.hpp"

#include "pmt/util/smct/symbol.hpp"

#include <cassert>

namespace pmt::parserbuilder {
 using namespace pmt::base;
 using namespace pmt::util::smct;
 using namespace pmt::util::smrt;

auto LexerTables::get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType {
 State const* state = _lexer_state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(symbol_)) : StateNrSink;
}

auto LexerTables::get_state_accepts(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 State const* state = _lexer_state_machine.get_state(state_nr_);
 return state ? state->get_accepts().get_chunks() : pmt::base::Bitset::ChunkSpanConst{};
}

auto LexerTables::get_accept_count() const -> size_t {
 return _terminal_data.size();
}

auto LexerTables::get_start_accept_index() const -> size_t {
 return _start_accept_index;
}

auto LexerTables::get_eoi_accept_index() const -> size_t {
 return _eoi_accept_index;
}

auto LexerTables::get_accept_index_label(size_t index_) const -> std::string {
 return _terminal_data[index_]._label;
}

auto LexerTables::get_accept_index_hide(size_t index_) const -> bool {
 return _terminal_data[index_]._hide;
}

auto LexerTables::get_accept_index_id(size_t index_) const -> GenericId::IdType {
 return _terminal_data[index_]._id;
}

auto LexerTables::id_to_string(GenericId::IdType id_) const -> std::string {
 return _id_names[id_ - get_min_id()];
}

auto LexerTables::get_id_count() const -> size_t {
 return _id_count;
}

auto LexerTables::get_linecount_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType {
 State const* state = _linecount_state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(symbol_)) : StateNrSink;
}

auto LexerTables::is_linecount_state_nr_accepting(StateNrType state_nr_) const -> bool {
 State const* state = _linecount_state_machine.get_state(state_nr_);
 return state ? state->get_accepts().any() : false;
}

auto LexerTables::terminal_label_to_index(std::string_view label_) const -> std::optional<size_t> {
 auto const itr = std::find_if(_terminal_data.begin(), _terminal_data.end(), [&](TerminalData const& terminal_data_) {
   return terminal_data_._label == label_;
 });
 
 return (itr != _terminal_data.end()) ? std::make_optional(std::distance(_terminal_data.begin(), itr)) : std::nullopt;
}

}