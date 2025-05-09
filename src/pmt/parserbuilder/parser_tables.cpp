#include "pmt/parserbuilder/parser_tables.hpp"

#include "pmt/util/smct/symbol.hpp"


namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smct;
using namespace pmt::util::smrt;

auto ParserTables::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {
 State const* state = _parser_state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(symbol_)) : StateNrSink;
}

auto ParserTables::get_state_nonterminal(pmt::util::smrt::StateNrType state_nr_) const -> size_t {
 State const* state = _parser_state_machine.get_state(state_nr_);
 if (!state) {
  return NonterminalIndexInvalid;
 }

 size_t const countl = state->get_accepts().countl(false);
 return (countl == state->get_accepts().size()) ? NonterminalIndexInvalid : countl;
}

auto ParserTables::get_nonterminal_count() const -> size_t {
 return _nonterminal_data.size();
}

auto ParserTables::get_nonterminal_label(size_t index_) const -> std::string {
 return _nonterminal_data[index_]._label;
}

auto ParserTables::get_nonterminal_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType {
 return _nonterminal_data[index_]._id;
}

auto ParserTables::id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string {
 return _id_names[id_];

}

}