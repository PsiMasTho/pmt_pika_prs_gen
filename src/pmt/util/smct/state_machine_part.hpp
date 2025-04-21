#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/smct/symbol.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <optional>
#include <unordered_set>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::util::smct {

class StateMachinePart {
 private:
  // -$ Data $-
  std::optional<pmt::util::smrt::StateNrType> _incoming_state_nr;
  std::unordered_map<pmt::util::smrt::StateNrType, std::unordered_set<Symbol>> _outgoing_symbol_transitions;
  std::unordered_set<pmt::util::smrt::StateNrType> _outgoing_epsilon_transitions;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  StateMachinePart() = default;
  explicit StateMachinePart(pmt::util::smrt::StateNrType incoming_state_nr_);

  // --$ Other $--
  void set_incoming_state_nr(pmt::util::smrt::StateNrType incoming_state_nr_);
  auto get_incoming_state_nr() const -> std::optional<pmt::util::smrt::StateNrType>;
  void clear_incoming_state_nr();

  void add_outgoing_symbol_transition(pmt::util::smrt::StateNrType state_nr_from_, Symbol symbol_);
  void add_outgoing_epsilon_transition(pmt::util::smrt::StateNrType state_nr_from_);

  void clear_outgoing_transitions();
  void merge_outgoing_transitions(StateMachinePart& other_);

  void connect_outgoing_transitions_to(pmt::util::smrt::StateNrType state_nr_to_, StateMachine& state_machine_);
};

}  // namespace pmt::util::smct