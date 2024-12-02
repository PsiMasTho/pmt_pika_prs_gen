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

  // - Public data-
  std::unordered_map<StateNrType, State> _states;

  // - Public functions -
  void prune(StateNrType state_nr_from_ = 0);
  void determinize();
  void minimize();

  auto get_unused_state_nr() const -> StateNrType;

 private:
  void subset();
  auto get_e_closure(StateNrType state_nr_from_, std::unordered_map<StateNrType, std::unordered_set<StateNrType>>& cache_) const -> std::unordered_set<StateNrType>;
  auto get_e_closure(std::unordered_set<StateNrType> const& state_nrs_from_, std::unordered_map<StateNrType, std::unordered_set<StateNrType>>& cache_) const -> std::unordered_set<StateNrType>;
  auto get_moves(std::unordered_set<StateNrType> const& state_nrs_from_, SymbolType symbol_) const -> std::unordered_set<StateNrType>;
  auto get_symbols(StateNrType state_nr_from_) const -> std::unordered_set<SymbolType>;
};

}  // namespace pmt::util::parse