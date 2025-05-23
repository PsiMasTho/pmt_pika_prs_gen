#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/symbol.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smct/state_machine.hpp"

#include <unordered_map>

namespace pmt::util::smct {

class StateMachineDeterminizer {
 private:
  std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::IntervalSet<pmt::util::smrt::StateNrType>> _e_closure_cache;
  std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::IntervalSet<pmt::util::smrt::StateNrType>> _new_to_old;
  std::unordered_map<pmt::base::IntervalSet<pmt::util::smrt::StateNrType>, pmt::util::smrt::StateNrType> _old_to_new;
  std::vector<pmt::util::smrt::StateNrType> _pending;
  StateMachine const& _input_state_machine;
  StateMachine _output_state_machine;
  pmt::util::smrt::StateNrType _state_nr_from = pmt::util::smrt::StateNrStart;

 public:
  explicit StateMachineDeterminizer(StateMachine const& state_machine_);
  auto set_state_nr_from(pmt::util::smrt::StateNrType state_nr_from_) && -> StateMachineDeterminizer&&;

  [[nodiscard]] auto determinize() && -> StateMachine;

 private:
  auto push_and_visit(pmt::base::IntervalSet<pmt::util::smrt::StateNrType> state_nr_set_) -> pmt::util::smrt::StateNrType;
  [[nodiscard]] auto take() -> pmt::util::smrt::StateNrType;
  auto get_e_closure(pmt::util::smrt::StateNrType state_nr_from_) -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;
  auto get_e_closure(pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const& state_nrs_from_) -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;
  auto get_moves(pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const& state_nrs_from_, Symbol symbol_) -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;
  auto get_symbols(pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const& state_nrs_from_) -> pmt::base::IntervalSet<pmt::util::smrt::SymbolType>;
};

}  // namespace pmt::util::smct