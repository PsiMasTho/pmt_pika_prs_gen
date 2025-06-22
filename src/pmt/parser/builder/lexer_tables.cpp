#include "pmt/parser/builder/lexer_tables.hpp"

#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/symbol.hpp"

#include <cassert>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;
using namespace pmt::parser::grammar;

namespace {
void create_bitsets_for_accepts(StateMachine const& state_machine_, std::unordered_map<StateNrType, Bitset>& accepts_bitsets_) {
  accepts_bitsets_.clear();
  state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State const* state = state_machine_.get_state(state_nr_);
    if (!state) {
      return;
    }

    Bitset& bitset = accepts_bitsets_[state_nr_];
    size_t const needed_size = (state->get_accepts().empty()) ? 0 : state->get_accepts().highest() + 1;
    bitset.resize(needed_size, false);
    state->get_accepts().for_each_key([&](AcceptsIndexType accept_index_) { bitset.set(accept_index_, true); });
  });
}
}  // namespace

auto LexerTables::get_state_nr_next(StateNrType state_nr_, SymbolValueType symbol_) const -> StateNrType {
  State const* state = _lexer_state_machine.get_state(state_nr_);
  return state ? state->get_symbol_transition(Symbol(SymbolKindCharacter, symbol_)) : StateNrSink;
}

auto LexerTables::get_state_accepts(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
  State const* state = _lexer_state_machine.get_state(state_nr_);
  if (!state) {
    return {};
  }

  auto const itr = _accepts_bitsets.find(state_nr_);
  return (itr != _accepts_bitsets.end()) ? itr->second.get_chunks() : Bitset::ChunkSpanConst{};
}

auto LexerTables::get_accept_count() const -> size_t {
  return _terminal_data.size();
}

auto LexerTables::get_start_accept_index() const -> size_t {
  return *terminal_name_to_index(Ast::TERMINAL_NAME_START);
}

auto LexerTables::get_eoi_accept_index() const -> size_t {
  return *terminal_name_to_index(Ast::NAME_EOI);
}

auto LexerTables::get_accept_index_display_name(size_t index_) const -> std::string {
  return _terminal_data[index_]._display_name;
}

auto LexerTables::get_accept_index_id(size_t index_) const -> GenericId::IdType {
  return _terminal_data[index_]._id;
}

auto LexerTables::get_linecount_state_nr_next(StateNrType state_nr_, SymbolValueType symbol_) const -> StateNrType {
  State const* state = _linecount_state_machine.get_state(state_nr_);
  return state ? state->get_symbol_transition(Symbol(SymbolKindCharacter, symbol_)) : StateNrSink;
}

auto LexerTables::is_linecount_state_nr_accepting(StateNrType state_nr_) const -> bool {
  State const* state = _linecount_state_machine.get_state(state_nr_);
  return state ? !state->get_accepts().empty() : false;
}

void LexerTables::set_lexer_state_machine(StateMachine state_machine_) {
  _lexer_state_machine = std::move(state_machine_);
  create_bitsets_for_accepts(_lexer_state_machine, _accepts_bitsets);
}

void LexerTables::set_linecount_state_machine(StateMachine state_machine_) {
  _linecount_state_machine = std::move(state_machine_);
}

auto LexerTables::get_lexer_state_machine() const -> pmt::util::sm::ct::StateMachine const& {
  return _lexer_state_machine;
}

auto LexerTables::get_linecount_state_machine() const -> pmt::util::sm::ct::StateMachine const& {
  return _linecount_state_machine;
}

auto LexerTables::get_accept_index_name(size_t index_) const -> std::string {
  return _terminal_data[index_]._name;
}

void LexerTables::add_terminal_data(std::string name_, std::string display_name_, GenericId::IdType id_value_) {
  _terminal_data.push_back(TerminalData{._name = std::move(name_), ._display_name = std::move(display_name_), ._id = id_value_});
}

auto LexerTables::terminal_name_to_index(std::string_view name_) const -> std::optional<size_t> {
  auto const itr = std::find_if(_terminal_data.begin(), _terminal_data.end(), [&](TerminalData const& terminal_data_) { return terminal_data_._name == name_; });

  return (itr != _terminal_data.end()) ? std::make_optional(std::distance(_terminal_data.begin(), itr)) : std::nullopt;
}

}  // namespace pmt::parser::builder