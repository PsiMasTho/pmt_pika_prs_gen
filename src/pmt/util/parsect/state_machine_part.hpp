#pragma once

#include "pmt/util/parsect/state_base.hpp"
#include "pmt/util/parsect/state_machine.hpp"

#include <optional>
#include <unordered_set>

namespace pmt::util::parsect {

template <IsStateTag TAG_>
class StateMachinePart {
 public:
  // - Lifetime -
  StateMachinePart() = default;
  StateMachinePart(State::StateNrType incoming_state_nr_);

  // - Other -
  void set_incoming_state_nr(State::StateNrType incoming_state_nr_);
  auto get_incoming_state_nr() const -> std::optional<State::StateNrType>;
  void clear_incoming_state_nr();

  void add_outgoing_symbol_transition(State::StateNrType state_nr_from_, size_t transition_kind_, StateBase::SymbolType symbol_);
  void add_outgoing_epsilon_transition(State::StateNrType state_nr_from_);

  void clear_outgoing_transitions();
  void merge_outgoing_transitions(StateMachinePart& other_);

  void connect_outgoing_transitions_to(State::StateNrType state_nr_to_, StateMachine<TAG_>& state_machine_);

 private:
  std::optional<State::StateNrType> _incoming_state_nr;
  std::array<std::unordered_map<State::StateNrType, std::unordered_set<StateBase::SymbolType>>, StateTraits<TAG_>::SymbolKindCount> _outgoing_symbol_transitions;
  std::unordered_set<State::StateNrType> _outgoing_epsilon_transitions;
};

}  // namespace pmt::util::parsect