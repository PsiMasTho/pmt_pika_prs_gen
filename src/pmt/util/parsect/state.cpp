#include "pmt/util/parsect/state.hpp"
#include <cstddef>

#include "pmt/base/algo.hpp"

namespace pmt::util::parsect {
using namespace pmt::base;

void State::add_epsilon_transition(StateNrType state_nr_) {
  _epsilon_transitions.insert(state_nr_);
}

void State::add_symbol_transition(Symbol symbol_, StateNrType state_nr_) {
  std::optional<size_t> const index_outer = binary_find_index(_symbols.begin(), _symbols.end(), symbol_._kind, [](auto const& lhs_, auto const& rhs_) { return symbol_kind_lt(lhs_, rhs_); });

  if (!index_outer.has_value()) {
    _symbols.emplace(_symbols.begin() + *index_outer);
    _symbol_transitions.emplace(_symbol_transitions.begin() + *index_outer);
  }

  std::optional<size_t> const index_inner = binary_find_index(_symbols[*index_outer].begin(), _symbols[*index_outer].end(), symbol_._value, [](auto const& lhs_, auto const& rhs_) { return symbol_value_lt(lhs_, rhs_); });

  if (!index_inner.has_value()) {
    _symbols[*index_outer].emplace(_symbols[*index_outer].begin() + *index_inner, symbol_);
    _symbol_transitions[*index_outer].emplace(_symbol_transitions[*index_outer].begin() + *index_inner, state_nr_);
  } else {
    _symbol_transitions[*index_outer][*index_inner] = state_nr_;
  }
}

void State::remove_epsilon_transition(StateNrType state_nr_) {
  _epsilon_transitions.erase(state_nr_);
}

void State::remove_symbol_transition(Symbol symbol_) {
  std::optional<size_t> const index_outer = binary_find_index(_symbols.begin(), _symbols.end(), symbol_._kind, [](auto const& lhs_, auto const& rhs_) { return symbol_kind_lt(lhs_, rhs_); });

  if (!index_outer.has_value()) {
    return;
  }

  std::optional<size_t> const index_inner = binary_find_index(_symbols[*index_outer].begin(), _symbols[*index_outer].end(), symbol_._value, [](auto const& lhs_, auto const& rhs_) { return symbol_value_lt(lhs_, rhs_); });

  if (!index_inner.has_value()) {
    return;
  }

  _symbols[*index_outer].erase(_symbols[*index_outer].begin() + *index_inner);
  _symbol_transitions[*index_outer].erase(_symbol_transitions[*index_outer].begin() + *index_inner);

  if (_symbols[*index_outer].empty()) {
    _symbols.erase(_symbols.begin() + *index_outer);
    _symbol_transitions.erase(_symbol_transitions.begin() + *index_outer);
  }
}

auto State::get_accepts() const -> pmt::base::Bitset const& {
  return _accepts;
}

auto State::get_accepts() -> pmt::base::Bitset& {
  return _accepts;
}

auto State::get_epsilon_transitions() const -> std::unordered_set<StateNrType> const& {
  return _epsilon_transitions;
}

auto State::get_symbol_transition(Symbol symbol_) const -> StateNrType {
  std::optional<size_t> const index_outer = binary_find_index(_symbols.begin(), _symbols.end(), symbol_._kind, [](auto const& lhs_, auto const& rhs_) { return symbol_kind_lt(lhs_, rhs_); });

  if (!index_outer.has_value()) {
    return StateNrSink;
  }

  std::optional<size_t> const index_inner = binary_find_index(_symbols[*index_outer].begin(), _symbols[*index_outer].end(), symbol_._value, [](auto const& lhs_, auto const& rhs_) { return symbol_value_lt(lhs_, rhs_); });

  return index_inner.has_value() ? _symbol_transitions[*index_outer][*index_inner] : StateNrSink;
}

auto State::get_symbols(Symbol::KindType kind_) const -> std::vector<Symbol> const& {
  std::optional<size_t> const index = binary_find_index(_symbols.begin(), _symbols.end(), kind_, [](auto const& lhs_, auto const& rhs_) { return symbol_kind_lt(lhs_, rhs_); });

  if (index.has_value()) {
    return _symbols[*index];
  } else {
    static std::vector<Symbol> const empty;
    return empty;
  }
}

auto State::get_symbol_kinds() const -> std::vector<Symbol::KindType> {
  std::vector<Symbol::KindType> ret;
  ret.reserve(_symbols.size());

  for (auto const& symbols : _symbols) {
    ret.push_back(symbols.front()._kind);
  }

  return ret;
}

auto State::symbol_value_lt(Symbol const& lhs_, Symbol const& rhs_) -> bool {
  return lhs_._value < rhs_._value;
}

}  // namespace pmt::util::parsect