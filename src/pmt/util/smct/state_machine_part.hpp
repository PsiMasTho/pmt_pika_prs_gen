#pragma once

#include "pmt/util/smct/state_machine.hpp"

#include <optional>
#include <unordered_set>

namespace pmt::util::smct {

class StateMachinePart {
 private:
  // -$ Data $-
  std::optional<State::StateNrType> _incoming_state_nr;
  std::unordered_map<State::StateNrType, std::unordered_set<Symbol>> _outgoing_symbol_transitions;
  std::unordered_set<State::StateNrType> _outgoing_epsilon_transitions;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  StateMachinePart() = default;
  StateMachinePart(State::StateNrType incoming_state_nr_);

  // --$ Other $--
  void set_incoming_state_nr(State::StateNrType incoming_state_nr_);
  auto get_incoming_state_nr() const -> std::optional<State::StateNrType>;
  void clear_incoming_state_nr();

  void add_outgoing_symbol_transition(State::StateNrType state_nr_from_, Symbol symbol_);
  void add_outgoing_epsilon_transition(State::StateNrType state_nr_from_);

  void clear_outgoing_transitions();
  void merge_outgoing_transitions(StateMachinePart& other_);

  void connect_outgoing_transitions_to(State::StateNrType state_nr_to_, StateMachine& state_machine_);
};

}  // namespace pmt::util::smct