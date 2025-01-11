#pragma once

#include "pmt/util/parsect/state_machine.hpp"
#include "pmt/util/parsect/symbol.hpp"

namespace pmt::util::parsect {

class StateMachineDeterminizer {
 public:
  static void determinize(StateMachine& state_machine_);

 private:
  static auto get_e_closure(StateMachine const& state_machine_, State::StateNrType state_nr_from_, std::unordered_map<State::StateNrType, std::unordered_set<State::StateNrType>>& cache_) -> std::unordered_set<State::StateNrType>;
  static auto get_e_closure(StateMachine const& state_machine_, std::unordered_set<State::StateNrType> const& state_nrs_from_, std::unordered_map<State::StateNrType, std::unordered_set<State::StateNrType>>& cache_) -> std::unordered_set<State::StateNrType>;
  static auto get_moves(StateMachine const& state_machine_, std::unordered_set<State::StateNrType> const& state_nrs_from_, Symbol symbol_) -> std::unordered_set<State::StateNrType>;
  static auto get_symbols(StateMachine const& state_machine_, std::unordered_set<State::StateNrType> const& state_nrs_from_) -> std::unordered_set<Symbol>;
};

}  // namespace pmt::util::parsect