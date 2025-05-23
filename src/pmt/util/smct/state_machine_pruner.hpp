#pragma once

#include "pmt/base/interval_map.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smct/state_machine.hpp"

#include <optional>
#include <vector>

namespace pmt::util::smct {

class StateMachinePruner {
 private:
  std::vector<pmt::util::smrt::StateNrType> _pending;
  pmt::base::IntervalMap<pmt::util::smrt::StateNrType, pmt::util::smrt::StateNrType> _visited;
  StateMachine const& _input_state_machine;
  StateMachine _output_state_machine;
  std::optional<pmt::util::smrt::StateNrType> _state_nr_from_new = std::nullopt;
  pmt::util::smrt::StateNrType _state_nr_from = pmt::util::smrt::StateNrStart;

 public:
  explicit StateMachinePruner(StateMachine const& state_machine_);
  auto set_state_nr_from(pmt::util::smrt::StateNrType state_nr_from_) && -> StateMachinePruner&&;
  auto enable_renumbering(pmt::util::smrt::StateNrType state_nr_from_new_ = pmt::util::smrt::StateNrStart) && -> StateMachinePruner&&;

  [[nodiscard]] auto prune() && -> StateMachine;

  private:
  auto push_and_visit(pmt::util::smrt::StateNrType state_nr_) -> pmt::util::smrt::StateNrType;
  [[nodiscard]] auto take() -> pmt::util::smrt::StateNrType;
  static void copy_accepts(pmt::util::smct::State const& state_old_, pmt::util::smct::State& state_new_);
  void follow_and_copy_epsilon_transitions(pmt::util::smct::State const& state_old_, pmt::util::smct::State& state_new_);
  void follow_and_copy_symbol_transitions(pmt::util::smct::State const& state_old_, pmt::util::smct::State& state_new_);
};

}  // namespace pmt::util::smct