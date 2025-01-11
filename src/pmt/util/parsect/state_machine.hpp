#pragma once

#include "pmt/util/parsect/state.hpp"

namespace pmt::util::parsect {

class StateMachine {
 private:
  // -$ Data $-
  std::vector<State::StateNrType> _state_nrs;
  std::vector<State> _states;

 public:
  // -$ Functions $-
  // --$ Other $--
  auto get_state(State::StateNrType state_nr_) -> State*;
  auto get_state(State::StateNrType state_nr_) const -> State const*;

  auto get_states() const -> std::vector<State> const&;
  auto get_state_nrs() const -> std::vector<State::StateNrType> const&;

  auto get_or_create_state(State::StateNrType state_nr_) -> State&;
  auto get_unused_state_nr() const -> State::StateNrType;
  auto create_new_state() -> State::StateNrType;

  void remove_state(State::StateNrType state_nr_);

  auto get_state_count() const -> size_t;
};

}  // namespace pmt::util::parsect