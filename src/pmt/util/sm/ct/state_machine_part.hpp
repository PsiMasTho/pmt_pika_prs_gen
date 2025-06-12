#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/base/interval_map.hpp"
#include "pmt/util/sm/ct/symbol.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <optional>
#include <unordered_set>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine)

namespace pmt::util::sm::ct {

class StateMachinePart {
 private:
  // -$ Data $-
  std::optional<StateNrType> _incoming_state_nr;
  std::unordered_map<StateNrType, std::unordered_map<SymbolKindType, pmt::base::IntervalSet<SymbolValueType>>> _outgoing_symbol_transitions;
  std::unordered_set<StateNrType> _outgoing_epsilon_transitions;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  StateMachinePart() = default;
  explicit StateMachinePart(StateNrType incoming_state_nr_);

  // --$ Other $--
  void set_incoming_state_nr(StateNrType incoming_state_nr_);
  auto get_incoming_state_nr() const -> std::optional<StateNrType>;
  void clear_incoming_state_nr();

  void add_outgoing_symbol_transition(StateNrType state_nr_from_, Symbol symbol_);
  void add_outgoing_symbol_transition(StateNrType state_nr_from_, SymbolKindType kind_, pmt::base::Interval<SymbolValueType> interval_);
  void add_outgoing_epsilon_transition(StateNrType state_nr_from_);

  void clear_outgoing_transitions();
  void merge_outgoing_transitions(StateMachinePart& other_);

  void connect_outgoing_transitions_to(StateNrType state_nr_to_, StateMachine& state_machine_);
};

}  // namespace pmt::util::smct