#include "pmt/util/parsect/state_machine.hpp"

#include "pmt/base/algo.hpp"

namespace pmt::util::parsect {
using namespace pmt::base;

auto StateMachine::get_state(State::StateNrType state_nr_) -> State* {
  std::optional<size_t> const idx = binary_find_index(_state_nrs.begin(), _state_nrs.end(), state_nr_, [](auto const& lhs_, auto const& rhs_) { return lhs_ < rhs_; });
  return (idx.has_value()) ? &_states[*idx] : nullptr;
}

auto StateMachine::get_state(State::StateNrType state_nr_) const -> State const* {
  std::optional<size_t> const idx = binary_find_index(_state_nrs.begin(), _state_nrs.end(), state_nr_, [](auto const& lhs_, auto const& rhs_) { return lhs_ < rhs_; });
  return (idx.has_value()) ? &_states[*idx] : nullptr;
}

auto StateMachine::get_states() const -> std::vector<State> const& {
  return _states;
}

auto StateMachine::get_state_nrs() const -> std::vector<State::StateNrType> const& {
  return _state_nrs;
}

auto StateMachine::get_or_create_state(State::StateNrType state_nr_) -> State& {
  auto const idx = binary_find_index(_state_nrs.begin(), _state_nrs.end(), state_nr_, [](auto const& lhs_, auto const& rhs_) { return lhs_ < rhs_; });

  if (idx.has_value()) {
    return _states[*idx];
  }

  _state_nrs.insert(_state_nrs.begin() + *idx, state_nr_);
  return *_states.emplace(_states.begin() + *idx);
}

auto StateMachine::get_unused_state_nr() const -> State::StateNrType {
  return _state_nrs.empty() ? 0 : _state_nrs.back() + 1;
}

auto StateMachine::create_new_state() -> State::StateNrType {
  State::StateNrType const state_nr = get_unused_state_nr();
  _state_nrs.push_back(state_nr);
  _states.emplace_back();
  return state_nr;
}

void StateMachine::remove_state(State::StateNrType state_nr_) {
  std::optional<size_t> const idx = binary_find_index(_state_nrs.begin(), _state_nrs.end(), state_nr_, [](auto const& lhs_, auto const& rhs_) { return lhs_ < rhs_; });

  if (!idx.has_value()) {
    return;
  }

  _state_nrs.erase(_state_nrs.begin() + *idx);
  _states.erase(_states.begin() + *idx);
}

auto StateMachine::get_state_count() const -> size_t {
  return _state_nrs.size();
}

}  // namespace pmt::util::parsect