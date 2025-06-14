#include "pmt/parser/builder/parser_tables.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/symbol.hpp"

#include <unordered_set>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
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

void fill_transition_masks(StateMachine const& state_machine_, std::unordered_map<StateNrType, Bitset>& transition_masks_, std::unordered_set<SymbolKindType> const& kinds_) {
  for (SymbolKindType const kind : kinds_) {
    state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
      State const& state = *state_machine_.get_state(state_nr_);
      IntervalMap<SymbolValueType, StateNrType> const transitions = state.get_symbol_transitions(kind);
      Bitset mask = transition_masks_[state_nr_];
      transitions.for_each_key([&](StateNrType, SymbolValueType symbol_) {
        if (mask.size() <= symbol_) {
          mask.resize(symbol_ + 1, false);
        }

        mask.set(symbol_, true);
      });
      transition_masks_[state_nr_] = mask;
    });
  }
}
}  // namespace

auto ParserTables::get_state_nr_next(StateNrType state_nr_, SymbolKindType kind_, SymbolValueType symbol_) const -> StateNrType {
  State const* state = _parser_state_machine.get_state(state_nr_);
  return state ? state->get_symbol_transition(Symbol(kind_, symbol_)) : StateNrSink;
}

auto ParserTables::get_state_terminal_transitions(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
  auto const itr = _parser_terminal_transition_masks.find(state_nr_);
  return (itr != _parser_terminal_transition_masks.end()) ? itr->second.get_chunks() : Bitset::ChunkSpanConst{};
}

auto ParserTables::get_state_hidden_terminal_transitions(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
  auto const itr = _parser_hidden_terminal_transition_masks.find(state_nr_);
  return (itr != _parser_hidden_terminal_transition_masks.end()) ? itr->second.get_chunks() : Bitset::ChunkSpanConst{};
}

auto ParserTables::get_state_conflict_transitions(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
  auto const itr = _parser_conflict_transition_masks.find(state_nr_);
  return (itr != _parser_conflict_transition_masks.end()) ? itr->second.get_chunks() : Bitset::ChunkSpanConst{};
}

auto ParserTables::get_state_kind(StateNrType state_nr_) const -> StateKind {
  State const* state = _parser_state_machine.get_state(state_nr_);
  assert(state);

  if (!state->get_accepts().empty()) {
    return StateTypeAccept;
  }

  if (state->get_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen)) != StateNrSink) {
    return StateTypeOpen;
  }

  if (!state->get_symbol_values(SymbolKindConflict).empty()) {
    return StateTypeConflict;
  }

  if (!state->get_symbol_values(SymbolKindTerminal).empty() || !state->get_symbol_values(SymbolKindHiddenTerminal).empty()) {
    return StateTypeTerminal;
  }

  pmt::unreachable();
}

auto ParserTables::get_state_accept_index(StateNrType state_nr_) const -> size_t {
  State const* state = _parser_state_machine.get_state(state_nr_);
  if (!state) {
    return AcceptIndexInvalid;
  }

  return (state->get_accepts().empty() ? AcceptIndexInvalid : state->get_accepts().lowest());
}

auto ParserTables::get_eoi_accept_index() const -> size_t {
  return *nonterminal_label_to_index(Ast::NAME_EOI);
}

auto ParserTables::get_accept_count() const -> size_t {
  return _nonterminal_data.size();
}

auto ParserTables::get_conflict_count() const -> size_t {
  size_t max = 0;

  for (auto const& [state_nr, mask] : _parser_conflict_transition_masks) {
    size_t const highest = mask.size() - mask.countr(false);
    max = std::max(max, highest);
  }

  return max;
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

auto ParserTables::get_accept_index_id(size_t index_) const -> GenericId::IdType {
  return _nonterminal_data[index_]._id;
}

auto ParserTables::get_lookahead_state_nr_next(StateNrType state_nr_, StateNrType symbol_) const -> StateNrType {
  State const* state = _lookahead_state_machine.get_state(state_nr_);
  return state ? state->get_symbol_transition(Symbol(SymbolKindTerminal, symbol_)) : StateNrSink;
}

auto ParserTables::get_lookahead_state_terminal_transitions(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
  auto const itr = _lookahead_terminal_transition_masks.find(state_nr_);
  return (itr != _lookahead_terminal_transition_masks.end()) ? itr->second.get_chunks() : Bitset::ChunkSpanConst{};
}

auto ParserTables::get_lookahead_state_accepts(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
  State const* state = _lookahead_state_machine.get_state(state_nr_);
  if (!state) {
    return {};
  }

  auto itr = _lookahead_accepts_bitsets.find(state_nr_);
  if (itr == _lookahead_accepts_bitsets.end()) {
    return {};
  }

  return itr->second.get_chunks();
}

void ParserTables::set_parser_state_machine(pmt::util::sm::ct::StateMachine state_machine_) {
  _parser_state_machine = std::move(state_machine_);
  fill_terminal_transition_masks();
  fill_hidden_terminal_transition_masks();
  fill_conflict_transition_masks();
}

void ParserTables::set_lookahead_state_machine(pmt::util::sm::ct::StateMachine state_machine_) {
  _lookahead_state_machine = std::move(state_machine_);
  create_bitsets_for_accepts(_lookahead_state_machine, _lookahead_accepts_bitsets);
  fill_lookahead_terminal_transition_masks();
}

auto ParserTables::get_parser_state_machine() const -> pmt::util::sm::ct::StateMachine const& {
  return _parser_state_machine;
}

auto ParserTables::get_lookahead_state_machine() const -> pmt::util::sm::ct::StateMachine const& {
  return _lookahead_state_machine;
}

void ParserTables::add_nonterminal_data(std::string label_, GenericId::IdType id_value_, bool merge_, bool unpack_, bool hide_) {
  _nonterminal_data.push_back(NonterminalData{._label = std::move(label_), ._id = id_value_, ._merge = merge_, ._unpack = unpack_, ._hide = hide_});
}

auto ParserTables::nonterminal_label_to_index(std::string_view label_) const -> std::optional<size_t> {
  auto const itr = std::find_if(_nonterminal_data.begin(), _nonterminal_data.end(), [&](NonterminalData const& terminal_data_) { return terminal_data_._label == label_; });

  return (itr != _nonterminal_data.end()) ? std::make_optional(std::distance(_nonterminal_data.begin(), itr)) : std::nullopt;
}

void ParserTables::fill_terminal_transition_masks() {
  fill_transition_masks(_parser_state_machine, _parser_terminal_transition_masks, {SymbolKindTerminal, SymbolKindHiddenTerminal});
}

void ParserTables::fill_hidden_terminal_transition_masks() {
  fill_transition_masks(_parser_state_machine, _parser_hidden_terminal_transition_masks, {SymbolKindHiddenTerminal});
}

void ParserTables::fill_conflict_transition_masks() {
  fill_transition_masks(_parser_state_machine, _parser_conflict_transition_masks, {SymbolKindConflict});
}

void ParserTables::fill_lookahead_terminal_transition_masks() {
  fill_transition_masks(_lookahead_state_machine, _lookahead_terminal_transition_masks, {SymbolKindTerminal});
}

}  // namespace pmt::parser::builder