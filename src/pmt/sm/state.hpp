#pragma once

#include "pmt/base/interval_map.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/sm/primitives.hpp"

namespace pmt::sm {

class State {
private:
 // -$ Data $-
 pmt::base::IntervalMap<SymbolType, StateNrType> _symbol_transitions;
 pmt::base::IntervalSet<StateNrType> _epsilon_transitions;
 pmt::base::IntervalSet<AcceptsIndexType> _accepts;

public:
 // -$ Functions $-
 // -$ Lifetime $-
 State() = default;

 // --$ Other $--
 void add_symbol_transition(SymbolType symbol_, StateNrType state_nr_);
 void add_symbol_transitions(pmt::base::Interval<SymbolType> symbol_interval_, StateNrType state_nr_);

 void remove_symbol_transition(SymbolType symbol_);
 void remove_symbol_transitions(pmt::base::Interval<SymbolType> symbol_interval_);
 void clear_symbol_transitions();

 auto get_symbol_transition(SymbolType symbol_) const -> StateNrType;
 auto get_symbol_transitions() const -> pmt::base::IntervalMap<SymbolType, StateNrType> const&;

 auto get_symbols() const -> pmt::base::IntervalSet<SymbolType>;

 void add_epsilon_transition(StateNrType state_nr_);
 void remove_epsilon_transition(StateNrType state_nr_);
 auto get_epsilon_transitions() const -> pmt::base::IntervalSet<StateNrType> const&;

 void add_accept(AcceptsIndexType accept_);
 void add_accepts(pmt::base::Interval<AcceptsIndexType> accepts_interval_);
 void add_accepts(pmt::base::IntervalSet<AcceptsIndexType> const& accepts_interval_set_);

 void remove_accept(AcceptsIndexType accept_);
 void remove_accepts(pmt::base::Interval<AcceptsIndexType> accepts_interval_);
 void remove_accepts(pmt::base::IntervalSet<AcceptsIndexType> const& accepts_interval_set_);

 auto get_accepts() const -> pmt::base::IntervalSet<AcceptsIndexType> const&;
};

}  // namespace pmt::sm
