#include "pmt/util/smct/state_machine_part.hpp"

#include "pmt/util/smct/state_machine.hpp"

namespace pmt::util::smct {

StateMachinePart::StateMachinePart(State::StateNrType incoming_state_nr_)
 : _incoming_state_nr(incoming_state_nr_) {
}

void StateMachinePart::set_incoming_state_nr(State::StateNrType incoming_state_nr_) {
  _incoming_state_nr = incoming_state_nr_;
}

auto StateMachinePart::get_incoming_state_nr() const -> std::optional<State::StateNrType> {
  return _incoming_state_nr;
}

void StateMachinePart::clear_incoming_state_nr() {
  _incoming_state_nr.reset();
}

void StateMachinePart::add_outgoing_symbol_transition(State::StateNrType state_nr_from_, Symbol symbol_) {
  _outgoing_symbol_transitions[state_nr_from_].insert(symbol_);
}

void StateMachinePart::add_outgoing_epsilon_transition(State::StateNrType state_nr_from_) {
  _outgoing_epsilon_transitions.insert(state_nr_from_);
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

void StateMachinePart::connect_outgoing_transitions_to(State::StateNrType state_nr_to_, StateMachine& state_machine_) {
  state_machine_.get_or_create_state(state_nr_to_);

  for (auto const& [state_nr_from, symbols] : _outgoing_symbol_transitions) {
    State& state_from = state_machine_.get_or_create_state(state_nr_from);
    for (auto const symbol : symbols) {
      state_from.add_symbol_transition(symbol, state_nr_to_);
    }
  }

  for (auto const state_nr_from : _outgoing_epsilon_transitions) {
    state_machine_.get_or_create_state(state_nr_from).add_epsilon_transition(state_nr_to_);
  }

  clear_outgoing_transitions();
}

}  // namespace pmt::util::smct