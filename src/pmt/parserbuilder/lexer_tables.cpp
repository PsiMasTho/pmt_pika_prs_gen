#include "pmt/parserbuilder/lexer_tables.hpp"

#include "pmt/util/smct/symbol.hpp"

namespace pmt::parserbuilder {
 using namespace pmt::base;
 using namespace pmt::util::smct;
 using namespace pmt::util::smrt;

auto LexerTables::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 State const* state = _lexer_state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(symbol_)) : StateNrSink;
}

auto LexerTables::get_state_terminals(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 State const* state = _lexer_state_machine.get_state(state_nr_);
 return state ? state->get_accepts().get_chunks() : pmt::base::Bitset::ChunkSpanConst{};
}

auto LexerTables::get_terminal_count() const -> size_t {
 return _terminal_data.size();
}

auto LexerTables::get_start_terminal_index() const -> size_t {
 return _start_accept_index;
}

auto LexerTables::get_eoi_terminal_index() const -> size_t {
 return _eoi_accept_index;
}

auto LexerTables::get_terminal_label(size_t index_) const -> std::string {
 return _terminal_data[index_]._label;
}

auto LexerTables::get_terminal_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType {
 return _terminal_data[index_]._id;
}

auto LexerTables::id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string {
 return _id_names[id_];
}

}