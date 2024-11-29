#pragma once

#include "pmt/util/parse/fa.hpp"

#include <optional>

namespace pmt::util::parse {

class FaPart {
 public:
  FaPart() = default;
  FaPart(Fa::StateNrType incoming_state_nr_);

  void set_incoming_state_nr(Fa::StateNrType incoming_state_nr_);
  auto get_incoming_state_nr() const -> std::optional<Fa::StateNrType>;
  void clear_incoming_state_nr();

  void add_outgoing_symbol_transition(Fa::StateNrType state_nr_from_, Fa::SymbolType symbol_);
  void add_outgoing_epsilon_transition(Fa::StateNrType state_nr_from_);

  void clear_outgoing_transitions();
  void merge_outgoing_transitions(FaPart& other_);

  void connect_outgoing_transitions_to(Fa::StateNrType state_nr_to_, Fa& fa_);

 private:
  std::optional<Fa::StateNrType> _incoming_state_nr;
  std::unordered_map<Fa::StateNrType, std::unordered_set<Fa::SymbolType>> _outgoing_symbol_transitions;
  std::unordered_set<Fa::StateNrType> _outgoing_epsilon_transitions;
};

}  // namespace pmt::util::parse