#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/base/interval_map.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/symbol.hpp"

#include <limits>

namespace pmt::util::smct {

class State {
 public:
  // -$ Types / Constants $-
  using StateNrType = uint64_t;

  enum : StateNrType {
    StateNrSink = std::numeric_limits<StateNrType>::max(),
  };

 private:
  // -$ Data $-
  pmt::base::IntervalSet<StateNrType> _epsilon_transitions;
  pmt::base::IntervalMap<Symbol::UnderlyingType, StateNrType> _symbol_transitions;
  pmt::base::Bitset _accepts;

 public:
  // -$ Functions $-
  // --$ Other $--
  void add_epsilon_transition(StateNrType state_nr_);
  void add_symbol_transition(Symbol symbol_, StateNrType state_nr_);

  void remove_epsilon_transition(StateNrType state_nr_);
  void remove_symbol_transition(Symbol symbol_);

  auto get_accepts() const -> pmt::base::Bitset const&;
  auto get_accepts() -> pmt::base::Bitset&;

  auto get_epsilon_transitions() const -> pmt::base::IntervalSet<StateNrType> const&;
  auto get_symbol_transition(Symbol symbol_) const -> StateNrType;

  auto get_symbols() const -> pmt::base::IntervalSet<Symbol::UnderlyingType>;
};

}  // namespace pmt::util::smct
