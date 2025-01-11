#include "pmt/util/parsect/state_machine.hpp"

namespace pmt::util::parsect {

auto StateMachine::get_state(State::StateNrType state_nr_) -> State* {
  auto const itr = _states.find(state_nr_);
  return (itr != _states.end()) ? &itr->second : nullptr;
}

auto StateMachine::get_state(State::StateNrType state_nr_) const -> State const* {
  auto const itr = _states.find(state_nr_);
  return (itr != _states.end()) ? &itr->second : nullptr;
}

auto StateMachine::get_or_create_state(State::StateNrType state_nr_) -> State& {
  return _states[state_nr_];
}

auto StateMachine::get_unused_state_nr() const -> State::StateNrType {
  State::StateNrType ret = 0;
  while (_states.contains(ret)) {
    ++ret;
  }
  return ret;
}

auto StateMachine::size() const -> size_t {
  return _states.size();
}

}  // namespace pmt::util::parsect