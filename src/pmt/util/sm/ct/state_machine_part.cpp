#include "pmt/util/sm/ct/state_machine_part.hpp"

#include "pmt/util/sm/ct/state_machine.hpp"

namespace pmt::util::sm::ct {
using namespace pmt::base;

StateMachinePart::StateMachinePart(StateNrType incoming_state_nr_)
 : _incoming_state_nr(incoming_state_nr_) {
}

void StateMachinePart::set_incoming_state_nr(StateNrType incoming_state_nr_) {
 _incoming_state_nr = incoming_state_nr_;
}

auto StateMachinePart::get_incoming_state_nr() const -> std::optional<StateNrType> {
 return _incoming_state_nr;
}

void StateMachinePart::clear_incoming_state_nr() {
 _incoming_state_nr.reset();
}

void StateMachinePart::add_outgoing_symbol_transition(StateNrType state_nr_from_, Symbol symbol_) {
 _outgoing_symbol_transitions[state_nr_from_][symbol_.get_kind()].insert(Interval(symbol_.get_value()));
}

void StateMachinePart::add_outgoing_symbol_transition(StateNrType state_nr_from_, SymbolKindType kind_, Interval<SymbolValueType> interval_) {
 _outgoing_symbol_transitions[state_nr_from_][kind_].insert(interval_);
}

void StateMachinePart::add_outgoing_epsilon_transition(StateNrType state_nr_from_) {
 _outgoing_epsilon_transitions.insert(state_nr_from_);
}

auto StateMachinePart::get_outgoing_symbol_transitions() const -> std::unordered_map<StateNrType, std::unordered_map<SymbolKindType, pmt::base::IntervalSet<SymbolValueType>>> const& {
 return _outgoing_symbol_transitions;
}

auto StateMachinePart::get_outgoing_epsilon_transitions() const -> std::unordered_set<StateNrType> const& {
 return _outgoing_epsilon_transitions;
}

void StateMachinePart::clear_outgoing_transitions() {
 _outgoing_symbol_transitions.clear();
 _outgoing_epsilon_transitions.clear();
}

void StateMachinePart::merge_outgoing_transitions(StateMachinePart& other_) {
 for (auto& [state_nr_from, symbols] : other_._outgoing_symbol_transitions) {
  _outgoing_symbol_transitions[state_nr_from].merge(symbols);
 }

 _outgoing_epsilon_transitions.merge(other_._outgoing_epsilon_transitions);

 other_.clear_outgoing_transitions();
}

void StateMachinePart::connect_outgoing_transitions_to(StateNrType state_nr_to_, StateMachine& state_machine_) {
 state_machine_.get_or_create_state(state_nr_to_);

 for (auto const& [state_nr_from, symbol_kinds_to_values] : _outgoing_symbol_transitions) {
  State& state_from = state_machine_.get_or_create_state(state_nr_from);
  for (auto const& [kind, values] : symbol_kinds_to_values) {
   values.for_each_interval([&](Interval<SymbolValueType> const& interval_) { state_from.add_symbol_transition(kind, interval_, state_nr_to_); });
  }
 }

 for (auto const state_nr_from : _outgoing_epsilon_transitions) {
  state_machine_.get_or_create_state(state_nr_from).add_epsilon_transition(state_nr_to_);
 }

 clear_outgoing_transitions();
}

}  // namespace pmt::util::sm::ct