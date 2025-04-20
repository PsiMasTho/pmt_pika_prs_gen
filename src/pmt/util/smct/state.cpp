#include "pmt/util/smct/state.hpp"

namespace pmt::util::smct {
using namespace pmt::base;

void State::add_epsilon_transition(StateNrType state_nr_) {
  _epsilon_transitions.insert(Interval<StateNrType>(state_nr_));
}

void State::add_symbol_transition(Symbol symbol_, StateNrType state_nr_) {
  _symbol_transitions.insert(Interval<StateNrType>(state_nr_), symbol_.get_combined());
}

void State::remove_epsilon_transition(StateNrType state_nr_) {
  _epsilon_transitions.erase(Interval<StateNrType>(state_nr_));
}

void State::remove_symbol_transition(Symbol symbol_) {
  _symbol_transitions.erase(Interval<StateNrType>(symbol_.get_combined()));
}

auto State::get_accepts() const -> Bitset const& {
  return _accepts;
}

auto State::get_accepts() -> Bitset& {
  return _accepts;
}

auto State::get_epsilon_transitions() const -> IntervalSet<StateNrType> const& {
  return _epsilon_transitions;
}

auto State::get_symbol_transition(Symbol symbol_) const -> StateNrType {
  Symbol::UnderlyingType const combined = symbol_.get_combined();
  StateNrType const* ret = _symbol_transitions.find(combined);
  return ret ? *ret : StateNrSink;
}

auto State::get_symbols() const -> IntervalSet<Symbol::UnderlyingType> {
  IntervalSet<Symbol::UnderlyingType> ret;
  for (size_t i = 0; i < _symbol_transitions.size(); ++i) {
    auto const entry = _symbol_transitions.get_by_index(i);
    ret.insert(entry._interval);
  }
  return ret;
}

}  // namespace pmt::util::smct