#include "pmt/util/sm/ct/state.hpp"

namespace pmt::util::sm::ct {
using namespace pmt::base;

namespace {
 static IntervalMap<SymbolValueType, StateNrType> const SYMBOL_TRANSITIONS_EMPTY;
}

void State::add_epsilon_transition(StateNrType state_nr_) {
  _epsilon_transitions.insert(Interval<StateNrType>(state_nr_));
}

void State::add_symbol_transition(Symbol symbol_, StateNrType state_nr_) {
  _symbol_transitions[symbol_.get_kind()].insert(Interval(symbol_.get_value()), state_nr_);
}

void State::add_symbol_transition(SymbolKindType kind_, Interval<SymbolValueType> interval_, StateNrType state_nr_) {
 _symbol_transitions[kind_].insert(interval_, state_nr_);
}

void State::remove_epsilon_transition(StateNrType state_nr_) {
  _epsilon_transitions.erase(Interval<StateNrType>(state_nr_));
}

void State::remove_symbol_transition(Symbol symbol_) {
 auto const itr = _symbol_transitions.find(symbol_.get_kind());
 if (itr == _symbol_transitions.end()) {
   return;
 }

 itr->second.erase(Interval(symbol_.get_value()));
}

void State::clear_symbol_transitions() {
 _symbol_transitions.clear();
}

auto State::get_accepts() const -> IntervalSet<AcceptsIndexType> const& {
 return _accepts;
}

auto State::get_accepts() -> IntervalSet<AcceptsIndexType>& {
 return _accepts;
}

auto State::get_epsilon_transitions() const -> IntervalSet<StateNrType> const& {
 return _epsilon_transitions;
}

auto State::get_symbol_transitions(SymbolKindType kind_) const -> IntervalMap<SymbolValueType, StateNrType> const& {
 auto const itr = _symbol_transitions.find(kind_);
 return (itr != _symbol_transitions.end()) ? itr->second : SYMBOL_TRANSITIONS_EMPTY;
}

auto State::get_symbol_transition(Symbol symbol_) const -> StateNrType {
 auto const itr = _symbol_transitions.find(symbol_.get_kind());
 if (itr == _symbol_transitions.end()) {
  return StateNrSink;
 }

 StateNrType const* ret = itr->second.find(symbol_.get_value());
 return (ret != nullptr) ? *ret : StateNrSink;
}

auto State::get_symbol_kinds() const -> IntervalSet<SymbolKindType> {
 IntervalSet<SymbolKindType> ret;
 for (auto const& [kind, _] : _symbol_transitions) {
  ret.insert(Interval(kind));
 }
 return ret;
}

auto State::get_symbol_values(SymbolKindType kind_) const -> IntervalSet<SymbolValueType> {
 IntervalSet<SymbolValueType> ret;
 auto const itr = _symbol_transitions.find(kind_);
 if (itr == _symbol_transitions.end()) {
  return ret;
 }

 itr->second.for_each_interval([&ret](auto const&, Interval<SymbolValueType> const& interval_) {
  ret.insert(interval_);
 });

 return ret;
}

}  // namespace pmt::util::smct