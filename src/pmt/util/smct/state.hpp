#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/base/interval_map.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/symbol.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

namespace pmt::util::smct {

class State {
 private:
  // -$ Data $-
  pmt::base::IntervalSet<pmt::util::smrt::StateNrType> _epsilon_transitions;
  pmt::base::IntervalMap<pmt::util::smrt::SymbolType, pmt::util::smrt::StateNrType> _symbol_transitions;
  pmt::base::Bitset _accepts;

 public:
  // -$ Functions $-
  // -$ Lifetime $-
  State() = default;

  // --$ Other $--
  void add_epsilon_transition(pmt::util::smrt::StateNrType state_nr_);
  void add_symbol_transition(Symbol symbol_, pmt::util::smrt::StateNrType state_nr_);
  void add_symbol_transition(pmt::base::Interval<pmt::util::smrt::SymbolType> interval_, pmt::util::smrt::StateNrType state_nr_);

  void remove_epsilon_transition(pmt::util::smrt::StateNrType state_nr_);
  void remove_symbol_transition(Symbol symbol_);
  void clear_symbol_transitions();

  auto get_accepts() const -> pmt::base::Bitset const&;
  auto get_accepts() -> pmt::base::Bitset&;

  auto get_epsilon_transitions() const -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const&;
  auto get_symbol_transitions() const -> pmt::base::IntervalMap<pmt::util::smrt::SymbolType, pmt::util::smrt::StateNrType> const&;
  auto get_symbol_transition(Symbol symbol_) const -> pmt::util::smrt::StateNrType;

  auto get_symbols() const -> pmt::base::IntervalSet<pmt::util::smrt::SymbolType>;
};

}  // namespace pmt::util::smct
