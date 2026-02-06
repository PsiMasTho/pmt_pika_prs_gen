#pragma once

#include "pmt/container/interval_map.hpp"
#include "pmt/container/interval_set.hpp"
#include "pmt/rt/primitives.hpp"

namespace pmt::builder {

class State {
private:
 // -$ Data $-
 pmt::container::IntervalMap<pmt::rt::SymbolType, pmt::rt::StateNrType> _symbol_transitions;
 pmt::container::IntervalSet<pmt::rt::StateNrType> _epsilon_transitions;
 pmt::container::IntervalSet<pmt::rt::IdType> _final_ids;

public:
 // -$ Functions $-
 // -$ Lifetime $-
 State() = default;

 // --$ Other $--
 void add_symbol_transition(pmt::rt::SymbolType symbol_, pmt::rt::StateNrType state_nr_);
 void add_symbol_transitions(pmt::container::Interval<pmt::rt::SymbolType> symbol_interval_, pmt::rt::StateNrType state_nr_);

 void remove_symbol_transition(pmt::rt::SymbolType symbol_);
 void remove_symbol_transitions(pmt::container::Interval<pmt::rt::SymbolType> symbol_interval_);
 void clear_symbol_transitions();

 auto get_symbol_transition(pmt::rt::SymbolType symbol_) const -> pmt::rt::StateNrType;
 auto get_symbol_transitions() const -> pmt::container::IntervalMap<pmt::rt::SymbolType, pmt::rt::StateNrType> const&;

 auto get_symbols() const -> pmt::container::IntervalSet<pmt::rt::SymbolType>;

 void add_epsilon_transition(pmt::rt::StateNrType state_nr_);
 void remove_epsilon_transition(pmt::rt::StateNrType state_nr_);
 auto get_epsilon_transitions() const -> pmt::container::IntervalSet<pmt::rt::StateNrType> const&;

 void add_final_id(pmt::rt::IdType accept_);
 void add_final_ids(pmt::container::Interval<pmt::rt::IdType> final_ids_interval_);
 void add_final_ids(pmt::container::IntervalSet<pmt::rt::IdType> const& final_ids_interval_set_);

 void remove_final_id(pmt::rt::IdType accept_);
 void remove_final_ids(pmt::container::Interval<pmt::rt::IdType> final_ids_interval_);
 void remove_final_ids(pmt::container::IntervalSet<pmt::rt::IdType> const& final_ids_interval_set_);

 auto get_final_ids() const -> pmt::container::IntervalSet<pmt::rt::IdType> const&;
};

}  // namespace pmt::builder
