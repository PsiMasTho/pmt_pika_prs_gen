#include "pmt/parserbuilder/lexer_tables.hpp"

#include "pmt/util/smct/symbol.hpp"

namespace pmt::parserbuilder {
 using namespace pmt::base;
 using namespace pmt::util::smct;
 using namespace pmt::util::smrt;

auto LexerTables::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 State const* state = get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(symbol_)) : StateNrSink;
}

auto LexerTables::get_state_terminals(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
 State const* state = get_state(state_nr_);
 return state ? state->get_accepts().get_chunks() : pmt::base::Bitset::ChunkSpanConst{};
}

auto LexerTables::get_terminal_count() const -> size_t {
 size_t max = 0;
 IntervalSet<StateNrType> state_nrs = get_state_nrs();
 state_nrs.for_each_key(
  [&](StateNrType state_nr_) {
   State const* state = get_state(state_nr_);
   Bitset const& accepts = state->get_accepts();
   size_t const furthest = accepts.size() - accepts.countr(false);
   max = std::max(max, furthest);
  });

  return max;
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