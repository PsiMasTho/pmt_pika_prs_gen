#include "pmt/sm/state_machine.hpp"

#include "pmt/base/algo.hpp"

namespace pmt::sm {
auto StateMachine::get_state(StateNrType state_nr_) -> State* {
 std::span<StateNrType> const state_nrs = _states.template get_span_by_type<StateNrType>();
 size_t const idx = pmt::base::binary_find_index(state_nrs.begin(), state_nrs.end(), state_nr_);
 return (idx == state_nrs.size()) ? nullptr : &_states.template get_span_by_type<State>()[idx];
}

auto StateMachine::get_state(StateNrType state_nr_) const -> State const* {
 std::span<StateNrType const> const state_nrs = _states.template get_span_by_type<StateNrType>();
 size_t const idx = pmt::base::binary_find_index(state_nrs.begin(), state_nrs.end(), state_nr_);
 return (idx == state_nrs.size()) ? nullptr : &_states.template get_span_by_type<State>()[idx];
}

auto StateMachine::get_state_nrs() const -> std::span<StateNrType const> {
 return _states.template get_span_by_type<StateNrType>();
}

auto StateMachine::get_or_create_state(StateNrType state_nr_) -> State& {
 std::span<StateNrType> const state_nrs = _states.template get_span_by_type<StateNrType>();
 size_t const idx = pmt::base::binary_find_index(state_nrs.begin(), state_nrs.end(), state_nr_);
 if (idx == state_nrs.size()) {
  _states.insert(idx, state_nr_, State());
 }
 return _states.template get_span_by_type<State>()[idx];
}

auto StateMachine::get_unused_state_nr() const -> StateNrType {
 return _states.empty() ? 0 : (_states.template get_span_by_type<StateNrType>().back() + 1);
}

auto StateMachine::create_new_state() -> StateNrType {
 StateNrType const state_nr = get_unused_state_nr();
 _states.push_back(state_nr, State());
 return state_nr;
}

void StateMachine::remove_state(StateNrType state_nr_) {
 std::span<StateNrType> const state_nrs = _states.template get_span_by_type<StateNrType>();
 size_t const idx = pmt::base::binary_find_index(state_nrs.begin(), state_nrs.end(), state_nr_);
 if (idx == state_nrs.size()) {
  return;
 }
 _states.erase(idx);
}

auto StateMachine::get_state_count() const -> size_t {
 return _states.size();
}

}  // namespace pmt::sm