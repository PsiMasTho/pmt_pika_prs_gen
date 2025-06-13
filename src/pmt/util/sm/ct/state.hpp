#pragma once

#include "pmt/base/interval_map.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/sm/ct/symbol.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <unordered_map>

namespace pmt::util::sm::ct {

class State {
 private:
  // -$ Data $-
  pmt::base::IntervalSet<StateNrType> _epsilon_transitions;
  std::unordered_map<SymbolKindType, pmt::base::IntervalMap<SymbolValueType, StateNrType>> _symbol_transitions;
  pmt::base::IntervalSet<AcceptsIndexType> _accepts;

 public:
  // -$ Functions $-
  // -$ Lifetime $-
  State() = default;

  // --$ Other $--
  void add_epsilon_transition(StateNrType state_nr_);
  void add_symbol_transition(Symbol symbol_, StateNrType state_nr_);
  void add_symbol_transition(SymbolKindType kind_, pmt::base::Interval<SymbolValueType> interval_, StateNrType state_nr_);

  void remove_epsilon_transition(StateNrType state_nr_);
  void remove_symbol_transition(Symbol symbol_);
  void clear_symbol_transitions();

  auto get_accepts() const -> pmt::base::IntervalSet<AcceptsIndexType> const&;
  auto get_accepts() -> pmt::base::IntervalSet<AcceptsIndexType>&;

  auto get_epsilon_transitions() const -> pmt::base::IntervalSet<StateNrType> const&;
  auto get_symbol_transitions(SymbolKindType kind_) const -> pmt::base::IntervalMap<SymbolValueType, StateNrType> const&;
  auto get_symbol_transition(Symbol symbol_) const -> StateNrType;

  auto get_symbol_kinds() const -> pmt::base::IntervalSet<SymbolKindType>;
  auto get_symbol_values(SymbolKindType kind_) const -> pmt::base::IntervalSet<SymbolValueType>;
};

}  // namespace pmt::util::sm::ct
