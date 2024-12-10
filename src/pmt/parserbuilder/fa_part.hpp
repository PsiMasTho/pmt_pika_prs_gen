#pragma once

#include "pmt/util/parsect/fa.hpp"

#include <optional>

namespace pmt::parserbuilder {

class FaPart {
 public:
  FaPart() = default;
  FaPart(pmt::util::parsect::Fa::StateNrType incoming_state_nr_);

  auto take() -> FaPart;

  void set_incoming_state_nr(pmt::util::parsect::Fa::StateNrType incoming_state_nr_);
  auto get_incoming_state_nr() const -> std::optional<pmt::util::parsect::Fa::StateNrType>;
  void clear_incoming_state_nr();

  void add_outgoing_symbol_transition(pmt::util::parsect::Fa::StateNrType state_nr_from_, pmt::util::parsect::Fa::SymbolType symbol_);
  void add_outgoing_epsilon_transition(pmt::util::parsect::Fa::StateNrType state_nr_from_);

  void clear_outgoing_transitions();
  void merge_outgoing_transitions(FaPart& other_);

  void connect_outgoing_transitions_to(pmt::util::parsect::Fa::StateNrType state_nr_to_, pmt::util::parsect::Fa& fa_);

 private:
  std::optional<pmt::util::parsect::Fa::StateNrType> _incoming_state_nr;
  std::unordered_map<pmt::util::parsect::Fa::StateNrType, std::unordered_set<pmt::util::parsect::Fa::SymbolType>> _outgoing_symbol_transitions;
  std::unordered_set<pmt::util::parsect::Fa::StateNrType> _outgoing_epsilon_transitions;
};

}  // namespace pmt::parserbuilder