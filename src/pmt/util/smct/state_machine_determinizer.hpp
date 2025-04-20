#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smct/symbol.hpp"

namespace pmt::util::smct {

class StateMachineDeterminizer {
 public:
  static void determinize(StateMachine& state_machine_);

 private:
  static auto get_e_closure(StateMachine const& state_machine_, State::StateNrType state_nr_from_, std::unordered_map<State::StateNrType, pmt::base::IntervalSet<State::StateNrType>>& cache_) -> pmt::base::IntervalSet<State::StateNrType>;
  static auto get_e_closure(StateMachine const& state_machine_, pmt::base::IntervalSet<State::StateNrType> const& state_nrs_from_, std::unordered_map<State::StateNrType, pmt::base::IntervalSet<State::StateNrType>>& cache_) -> pmt::base::IntervalSet<State::StateNrType>;
  static auto get_moves(StateMachine const& state_machine_, pmt::base::IntervalSet<State::StateNrType> const& state_nrs_from_, Symbol symbol_) -> pmt::base::IntervalSet<State::StateNrType>;
  static auto get_symbols(StateMachine const& state_machine_, pmt::base::IntervalSet<State::StateNrType> const& state_nrs_from_) -> pmt::base::IntervalSet<Symbol::UnderlyingType>;
};

}  // namespace pmt::util::smct