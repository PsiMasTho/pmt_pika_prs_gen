#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/state.hpp"

#include <map>

namespace pmt::util::smct {

class StateMachine {
 private:
  // -$ Data $-
  std::map<State::StateNrType, State> _states;

 public:
  // -$ Functions $-
  // --$ Other $--
  auto get_state(State::StateNrType state_nr_) -> State*;
  auto get_state(State::StateNrType state_nr_) const -> State const*;

  auto get_state_nrs() const -> pmt::base::IntervalSet<State::StateNrType>;

  auto get_or_create_state(State::StateNrType state_nr_) -> State&;
  auto get_unused_state_nr() const -> State::StateNrType;
  auto create_new_state() -> State::StateNrType;

  void remove_state(State::StateNrType state_nr_);

  auto get_state_count() const -> size_t;
};

}  // namespace pmt::util::smct