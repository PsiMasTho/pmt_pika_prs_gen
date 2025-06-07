#pragma once

#include "pmt/base/interval_map.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/symbol.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <unordered_map>

namespace pmt::util::smct {

class State {
 private:
  // -$ Data $-
  pmt::base::IntervalSet<pmt::util::smrt::StateNrType> _epsilon_transitions;
  std::unordered_map<pmt::util::smrt::SymbolKindType, pmt::base::IntervalMap<pmt::util::smrt::SymbolValueType, pmt::util::smrt::StateNrType>> _symbol_transitions;
  pmt::base::IntervalSet<pmt::util::smrt::AcceptsIndexType> _accepts;

 public:
  // -$ Functions $-
  // -$ Lifetime $-
  State() = default;

  // --$ Other $--
  void add_epsilon_transition(pmt::util::smrt::StateNrType state_nr_);
  void add_symbol_transition(Symbol symbol_, pmt::util::smrt::StateNrType state_nr_);
  void add_symbol_transition(pmt::util::smrt::SymbolKindType kind_, pmt::base::Interval<pmt::util::smrt::SymbolValueType> interval_, pmt::util::smrt::StateNrType state_nr_);

  void remove_epsilon_transition(pmt::util::smrt::StateNrType state_nr_);
  void remove_symbol_transition(Symbol symbol_);
  void clear_symbol_transitions();

  auto get_accepts() const -> pmt::base::IntervalSet<pmt::util::smrt::AcceptsIndexType> const&;
  auto get_accepts() -> pmt::base::IntervalSet<pmt::util::smrt::AcceptsIndexType>&;

  auto get_epsilon_transitions() const -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const&;
  auto get_symbol_transitions(pmt::util::smrt::SymbolKindType kind_) const -> pmt::base::IntervalMap<pmt::util::smrt::SymbolValueType, pmt::util::smrt::StateNrType> const&;
  auto get_symbol_transition(Symbol symbol_) const -> pmt::util::smrt::StateNrType;

  auto get_symbol_kinds() const -> pmt::base::IntervalSet<pmt::util::smrt::SymbolKindType>;
  auto get_symbol_values(pmt::util::smrt::SymbolKindType kind_) const -> pmt::base::IntervalSet<pmt::util::smrt::SymbolValueType>;
};

}  // namespace pmt::util::smct
