#include "pmt/util/smct/state_machine.hpp"

namespace pmt::util::smct {
using namespace pmt::base;

auto StateMachine::get_state(State::StateNrType state_nr_) -> State* {
  auto it = _states.find(state_nr_);
  if (it == _states.end()) {
    return nullptr;
  }
  return &it->second;
}

auto StateMachine::get_state(State::StateNrType state_nr_) const -> State const* {
  auto it = _states.find(state_nr_);
  if (it == _states.end()) {
    return nullptr;
  }
  return &it->second;
}

auto StateMachine::get_state_nrs() const -> IntervalSet<State::StateNrType> {
  IntervalSet<State::StateNrType> ret;
  for (auto const& [state_nr, _] : _states) {
    ret.insert(Interval<State::StateNrType>(state_nr));
  }
  return ret;
}

auto StateMachine::get_or_create_state(State::StateNrType state_nr_) -> State& {
  return _states[state_nr_];
}

auto StateMachine::get_unused_state_nr() const -> State::StateNrType {
  return _states.empty() ? 0 : std::prev(_states.end())->first + 1;
}

auto StateMachine::create_new_state() -> State::StateNrType {
  State::StateNrType const state_nr = get_unused_state_nr();
  _states.emplace(state_nr, State());
  return state_nr;
}

void StateMachine::remove_state(State::StateNrType state_nr_) {
  _states.erase(state_nr_);
}

auto StateMachine::get_state_count() const -> size_t {
  return _states.size();
}

}  // namespace pmt::util::smct