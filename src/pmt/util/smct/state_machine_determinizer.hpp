#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/util/smct/symbol.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::util::smct {

class StateMachineDeterminizer {
 public:
  static void determinize(StateMachine& state_machine_);

 private:
  static auto get_e_closure(StateMachine const& state_machine_, pmt::util::smrt::StateNrType state_nr_from_, std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::IntervalSet<pmt::util::smrt::StateNrType>>& cache_) -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;
  static auto get_e_closure(StateMachine const& state_machine_, pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const& state_nrs_from_, std::unordered_map<pmt::util::smrt::StateNrType, pmt::base::IntervalSet<pmt::util::smrt::StateNrType>>& cache_) -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;
  static auto get_moves(StateMachine const& state_machine_, pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const& state_nrs_from_, Symbol symbol_) -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;
  static auto get_symbols(StateMachine const& state_machine_, pmt::base::IntervalSet<pmt::util::smrt::StateNrType> const& state_nrs_from_) -> pmt::base::IntervalSet<pmt::util::smrt::SymbolType>;
};

}  // namespace pmt::util::smct