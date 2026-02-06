#include "pmt/builder/state.hpp"

namespace pmt::builder {
using namespace pmt::container;
using namespace pmt::rt;

void State::add_symbol_transition(SymbolType symbol_, StateNrType state_nr_) {
 _symbol_transitions.insert(Interval(symbol_), state_nr_);
}

void State::add_symbol_transitions(Interval<SymbolType> symbol_interval_, StateNrType state_nr_) {
 _symbol_transitions.insert(symbol_interval_, state_nr_);
}

void State::remove_symbol_transition(SymbolType symbol_) {
 _symbol_transitions.erase(Interval(symbol_));
}

void State::remove_symbol_transitions(Interval<SymbolType> symbol_interval_) {
 _symbol_transitions.erase(symbol_interval_);
}

void State::clear_symbol_transitions() {
 _symbol_transitions.clear();
}

auto State::get_symbol_transition(SymbolType symbol_) const -> StateNrType {
 StateNrType const* state_nr = _symbol_transitions.find(symbol_);
 return state_nr ? *state_nr : StateNrInvalid;
}

auto State::get_symbol_transitions() const -> IntervalMap<SymbolType, StateNrType> const& {
 return _symbol_transitions;
}

auto State::get_symbols() const -> IntervalSet<SymbolType> {
 return _symbol_transitions.get_keys();
}

void State::add_epsilon_transition(StateNrType state_nr_) {
 _epsilon_transitions.insert(Interval<StateNrType>(state_nr_));
}

void State::remove_epsilon_transition(StateNrType state_nr_) {
 _epsilon_transitions.erase(Interval<StateNrType>(state_nr_));
}

auto State::get_epsilon_transitions() const -> IntervalSet<StateNrType> const& {
 return _epsilon_transitions;
}

void State::add_final_id(IdType accept_) {
 _final_ids.insert(Interval(accept_));
}

void State::add_final_ids(Interval<IdType> final_ids_interval_) {
 _final_ids.insert(final_ids_interval_);
}

void State::add_final_ids(IntervalSet<IdType> const& final_ids_interval_set_) {
 _final_ids.inplace_or(final_ids_interval_set_);
}

void State::remove_final_id(IdType accept_) {
 _final_ids.erase(Interval(accept_));
}

void State::remove_final_ids(Interval<IdType> final_ids_interval_) {
 _final_ids.erase(final_ids_interval_);
}

void State::remove_final_ids(IntervalSet<IdType> const& final_ids_interval_set_) {
 _final_ids.inplace_asymmetric_difference(final_ids_interval_set_);
}

auto State::get_final_ids() const -> IntervalSet<IdType> const& {
 return _final_ids;
}

}  // namespace pmt::builder