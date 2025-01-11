#pragma once

#include "pmt/util/parsect/state.hpp"

namespace pmt::util::parsect {

class StateMachine {
 public:
  // - Public functions -
  // -- Other --
  auto get_state(State::StateNrType state_nr_) -> State*;
  auto get_state(State::StateNrType state_nr_) const -> State const*;

  auto get_or_create_state(State::StateNrType state_nr_) -> State&;
  auto get_unused_state_nr() const -> State::StateNrType;
  auto create_new_state() -> State::StateNrType;

  auto size() const -> size_t;

 private:
  std::unordered_map<State::StateNrType, State> _states;
};

}  // namespace pmt::util::parsect