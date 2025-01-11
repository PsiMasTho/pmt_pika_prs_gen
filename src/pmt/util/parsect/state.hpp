#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsect/symbol.hpp"

#include <optional>
#include <unordered_set>
#include <vector>

namespace pmt::util::parsect {

class State {
 public:
  // -$ Types / Constants $-
  using StateNrType = size_t;

 private:
  // -$ Data $-
  std::unordered_set<StateNrType> _epsilon_transitions;
  std::vector<std::vector<StateNrType>> _symbol_transitions;
  std::vector<std::vector<Symbol>> _symbols;
  pmt::base::DynamicBitset _accepts;

 public:
  // -$ Functions $-
  // --$ Other $--
  void add_epsilon_transition(StateNrType state_nr_);
  void add_symbol_transition(Symbol symbol_, StateNrType state_nr_);

  void remove_epsilon_transition(StateNrType state_nr_);
  void remove_symbol_transition(Symbol symbol_);

  void add_accept(size_t accept_nr_);
  void remove_accept(size_t accept_nr_);
  auto has_accept(size_t accept_nr_) const -> bool;

  auto get_epsilon_transitions() const -> std::unordered_set<StateNrType> const&;
  auto get_symbol_transition(Symbol symbol_) const -> std::optional<StateNrType>;

  auto get_symbols(Symbol::KindType kind_) const -> std::vector<Symbol> const&;
  auto get_symbol_kinds() const -> std::vector<Symbol::KindType>;

 private:
  template <typename T_>
  static auto fetch_symbol_kind(T_ const& item_) -> Symbol::KindType;
  static auto symbol_kind_lt(auto const& lhs_, auto const& rhs_) -> bool;
  static auto symbol_value_lt(Symbol const& lhs_, Symbol const& rhs_) -> bool;
};

}  // namespace pmt::util::parsect

#include "pmt/util/parsect/state-inl.hpp"