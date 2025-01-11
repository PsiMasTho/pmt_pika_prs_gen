// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/util/parsect/state_machine_part.hpp"
#endif
// clang-format on

#include "pmt/util/parsect/state_machine.hpp"

namespace pmt::util::parsect {

template <IsStateTag TAG_>
StateMachinePart<TAG_>::StateMachinePart(State::StateNrType incoming_state_nr_)
 : _incoming_state_nr(incoming_state_nr_) {
}

template <IsStateTag TAG_>
void StateMachinePart<TAG_>::set_incoming_state_nr(State::StateNrType incoming_state_nr_) {
  _incoming_state_nr = incoming_state_nr_;
}

template <IsStateTag TAG_>
auto StateMachinePart<TAG_>::get_incoming_state_nr() const -> std::optional<State::StateNrType> {
  return _incoming_state_nr;
}

template <IsStateTag TAG_>
void StateMachinePart<TAG_>::clear_incoming_state_nr() {
  _incoming_state_nr.reset();
}

template <IsStateTag TAG_>
void StateMachinePart<TAG_>::add_outgoing_symbol_transition(State::StateNrType state_nr_from_, size_t transition_kind_, StateBase::SymbolType symbol_) {
  _outgoing_symbol_transitions[transition_kind_][state_nr_from_].insert(symbol_);
}

template <IsStateTag TAG_>
void StateMachinePart<TAG_>::add_outgoing_epsilon_transition(State::StateNrType state_nr_from_) {
  _outgoing_epsilon_transitions.insert(state_nr_from_);
}

template <IsStateTag TAG_>
void StateMachinePart<TAG_>::clear_outgoing_transitions() {
  _outgoing_symbol_transitions.clear();
  _outgoing_epsilon_transitions.clear();
}

template <IsStateTag TAG_>
void StateMachinePart<TAG_>::merge_outgoing_transitions(StateMachinePart& other_) {
  for (auto& [state_nr_from, symbols] : other_._outgoing_symbol_transitions) {
    _outgoing_symbol_transitions[state_nr_from].merge(symbols);
  }

  _outgoing_epsilon_transitions.merge(other_._outgoing_epsilon_transitions);

  other_.clear_outgoing_transitions();
}

template <IsStateTag TAG_>
void StateMachinePart<TAG_>::connect_outgoing_transitions_to(State::StateNrType state_nr_to_, StateMachine<TAG_>& state_machine_) {
  state_machine_.get_or_create_state(state_nr_to_);

  for (size_t i = 0; i < _outgoing_symbol_transitions.size(); ++i) {
    for (auto const& [state_nr_from, symbols] : _outgoing_symbol_transitions[i]) {
      for (auto& symbol : symbols) {
        state_machine_.get_or_create_state(state_nr_from)._symbol_transitions[symbol.get_type()][symbol] = state_nr_to_;
      }
    }
  }

  for (auto const state_nr_from : _outgoing_epsilon_transitions) {
    state_machine_.get_or_create_state(state_nr_from)._epsilon_transitions.insert(state_nr_to_);
  }

  clear_outgoing_transitions();
}

}  // namespace pmt::util::parsect