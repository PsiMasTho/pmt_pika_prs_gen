#include "pmt/builder/state_machine.hpp"

#include "pmt/util/algo.hpp"

namespace pmt::builder {
using namespace pmt::rt;

auto StateMachine::get_state(StateNrType state_nr_) -> State* {
 size_t const idx = pmt::util::binary_find_index(_state_nrs.begin(), _state_nrs.end(), state_nr_);
 return (idx == _state_nrs.size()) ? nullptr : &_states[idx];
}

auto StateMachine::get_state(StateNrType state_nr_) const -> State const* {
 std::span<StateNrType const> const state_nrs = _state_nrs;
 size_t const idx = pmt::util::binary_find_index(state_nrs.begin(), state_nrs.end(), state_nr_);
 return (idx == state_nrs.size()) ? nullptr : &_states[idx];
}

auto StateMachine::get_state_nrs() const -> std::span<StateNrType const> {
 return _state_nrs;
}

auto StateMachine::get_or_create_state(StateNrType state_nr_) -> State& {
 std::span<StateNrType> const state_nrs = _state_nrs;
 size_t const idx = pmt::util::binary_find_index(state_nrs.begin(), state_nrs.end(), state_nr_);
 if (idx == state_nrs.size()) {
  _state_nrs.insert(_state_nrs.begin() + idx, state_nr_);
  _states.emplace(_states.begin() + idx);
 }
 return _states[idx];
}

auto StateMachine::get_unused_state_nr() const -> StateNrType {
 return _states.empty() ? 0 : (_state_nrs.back() + 1);
}

auto StateMachine::create_new_state() -> StateNrType {
 StateNrType const state_nr = get_unused_state_nr();
 _state_nrs.push_back(state_nr);
 _states.emplace_back();
 return state_nr;
}

void StateMachine::remove_state(StateNrType state_nr_) {
 std::span<StateNrType> const state_nrs = _state_nrs;
 size_t const idx = pmt::util::binary_find_index(state_nrs.begin(), state_nrs.end(), state_nr_);
 if (idx == state_nrs.size()) {
  return;
 }
 _state_nrs.erase(_state_nrs.begin() + idx);
 _states.erase(_states.begin() + idx);
}

auto StateMachine::get_state_count() const -> size_t {
 return _states.size();
}

}  // namespace pmt::builder