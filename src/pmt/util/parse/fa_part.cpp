#include "pmt/util/parse/fa_part.hpp"

namespace pmt::util::parse {

FaPart::FaPart(Fa::StateNrType incoming_state_nr_)
 : _incoming_state_nr(incoming_state_nr_) {
}

void FaPart::set_incoming_state_nr(Fa::StateNrType incoming_state_nr_) {
  _incoming_state_nr = incoming_state_nr_;
}

auto FaPart::get_incoming_state_nr() const -> std::optional<Fa::StateNrType> {
  return _incoming_state_nr;
}

void FaPart::clear_incoming_state_nr() {
  _incoming_state_nr.reset();
}

void FaPart::add_outgoing_symbol_transition(Fa::StateNrType state_nr_from_, Fa::SymbolType symbol_) {
  _outgoing_symbol_transitions[state_nr_from_].insert(symbol_);
}

void FaPart::add_outgoing_epsilon_transition(Fa::StateNrType state_nr_from_) {
  _outgoing_epsilon_transitions.insert(state_nr_from_);
}

void FaPart::clear_outgoing_transitions() {
  _outgoing_symbol_transitions.clear();
  _outgoing_epsilon_transitions.clear();
}

void FaPart::merge_outgoing_transitions(FaPart& other_) {
  for (auto& [state_nr_from, symbols] : other_._outgoing_symbol_transitions) {
    _outgoing_symbol_transitions[state_nr_from].merge(symbols);
  }

  _outgoing_epsilon_transitions.merge(other_._outgoing_epsilon_transitions);

  other_.clear_outgoing_transitions();
}

void FaPart::connect_outgoing_transitions_to(Fa::StateNrType state_nr_to_, Fa& fa_) {
  // In case it doesn't exist yet
  fa_._states[state_nr_to_];

  for (auto const& [state_nr_from, symbols] : _outgoing_symbol_transitions) {
    for (auto const symbol : symbols) {
      fa_._states[state_nr_from]._transitions._symbol_transitions[symbol] = state_nr_to_;
    }
  }

  for (auto const state_nr_from : _outgoing_epsilon_transitions) {
    fa_._states[state_nr_from]._transitions._epsilon_transitions.insert(state_nr_to_);
  }

  clear_outgoing_transitions();
}

}  // namespace pmt::util::parse