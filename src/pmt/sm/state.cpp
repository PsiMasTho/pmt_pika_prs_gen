#include "pmt/sm/state.hpp"

namespace pmt::sm {
using namespace pmt::base;

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

auto State::get_symbol_transitions() const -> pmt::base::IntervalMap<SymbolType, StateNrType> const& {
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

void State::add_accept(AcceptsIndexType accept_) {
 _accepts.insert(Interval(accept_));
}

void State::add_accepts(pmt::base::Interval<AcceptsIndexType> accepts_interval_) {
 _accepts.insert(accepts_interval_);
}

void State::add_accepts(pmt::base::IntervalSet<AcceptsIndexType> const& accepts_interval_set_) {
 _accepts.inplace_or(accepts_interval_set_);
}

void State::remove_accept(AcceptsIndexType accept_) {
 _accepts.erase(Interval(accept_));
}

void State::remove_accepts(pmt::base::Interval<AcceptsIndexType> accepts_interval_) {
 _accepts.erase(accepts_interval_);
}

void State::remove_accepts(pmt::base::IntervalSet<AcceptsIndexType> const& accepts_interval_set_) {
 _accepts.inplace_asymmetric_difference(accepts_interval_set_);
}

auto State::get_accepts() const -> IntervalSet<AcceptsIndexType> const& {
 return _accepts;
}

}  // namespace pmt::sm