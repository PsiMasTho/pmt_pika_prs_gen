#pragma once

#include "pmt/base/dynamic_bitset.hpp"

#include <unordered_map>
#include <unordered_set>

namespace pmt::util::parse {

class Fa {
 public:
  // - Public types and constants -
  using SymbolType = uint64_t;
  using StateNrType = size_t;

  class Transitions {
   public:
    std::unordered_map<SymbolType, StateNrType> _symbol_transitions;
    std::unordered_set<StateNrType> _epsilon_transitions;
  };

  class State {
   public:
    Transitions _transitions;
    pmt::base::DynamicBitset _accepts;
  };

  using GenericTransitions = std::unordered_map<StateNrType, std::unordered_set<StateNrType>>;

  // - Public data-
  std::unordered_map<StateNrType, State> _states;

  // - Public functions -
  void prune(StateNrType state_nr_from_ = 0);
  void prune_and_renumber(StateNrType state_nr_from_ = 0, StateNrType state_nr_from_new_ = 0);
  void determinize();
  void minimize();

  auto get_unused_state_nr() const -> StateNrType;
  auto get_backward_transitions() -> GenericTransitions;
  auto get_forward_transitions() -> GenericTransitions;

  static auto get_reachable_state_nrs(std::unordered_set<StateNrType> state_nr_from_) -> std::unordered_set<StateNrType>;

 private:
  auto get_e_closure(std::unordered_set<StateNrType> const& state_nrs_) const -> std::unordered_set<StateNrType>;
  auto get_moves(std::unordered_set<StateNrType> const& state_nrs_, SymbolType symbol_) const -> std::unordered_set<StateNrType>;
};

}  // namespace pmt::util::parse