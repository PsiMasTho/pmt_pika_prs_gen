#include "pmt/util/smct/state_machine.hpp"

#include <iostream>  // REMOVE ME

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

auto StateMachine::get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType {
  auto it = _states.find(state_nr_);
  if (it == _states.end()) {
    return StateNrSink;
  }

  State const& state = it->second;
  return state.get_symbol_transition(Symbol(symbol_));
}

auto StateMachine::get_state_accepts(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {
  auto it = _states.find(state_nr_);
  if (it == _states.end()) {
    return {};
  }
  return it->second.get_accepts().get_chunks();
}

auto StateMachine::get_accept_count() const -> size_t {
  size_t max = 0;
  for (auto const& [state_nr, state] : _states) {
    Bitset const& accepts = state.get_accepts();

    size_t const furthest = accepts.countl(false);
    max = (furthest != accepts.size()) ? std::max(max, furthest) : max;
  }
  return max;
}

auto StateMachine::get_state(StateNrType state_nr_) -> State* {
  auto it = _states.find(state_nr_);
  if (it == _states.end()) {
    return nullptr;
  }
  return &it->second;
}

auto StateMachine::get_state(StateNrType state_nr_) const -> State const* {
  auto it = _states.find(state_nr_);
  if (it == _states.end()) {
    return nullptr;
  }
  return &it->second;
}

auto StateMachine::get_state_nrs() const -> IntervalSet<StateNrType> {
  IntervalSet<StateNrType> ret;
  for (auto const& [state_nr, _] : _states) {
    ret.insert(Interval<StateNrType>(state_nr));
  }
  return ret;
}

auto StateMachine::get_or_create_state(StateNrType state_nr_) -> State& {
  return _states[state_nr_];
}

auto StateMachine::get_unused_state_nr() const -> StateNrType {
  return _states.empty() ? 0 : std::prev(_states.end())->first + 1;
}

auto StateMachine::create_new_state() -> StateNrType {
  StateNrType const state_nr = get_unused_state_nr();
  _states.emplace(state_nr, State());
  return state_nr;
}

void StateMachine::remove_state(StateNrType state_nr_) {
  _states.erase(state_nr_);
}

auto StateMachine::get_state_count() const -> size_t {
  return _states.size();
}

void StateMachine::debug_dump() {
  IntervalSet<StateNrType> const state_nrs = get_state_nrs();

  std::cerr << "DEBUG DUMPING STATE MACHINE, size: " << state_nrs.size() << std::endl;

  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<StateNrType> const state_nr_interval = state_nrs.get_by_index(i);
    for (StateNrType state_nr = state_nr_interval.get_lower(); state_nr <= state_nr_interval.get_upper(); ++state_nr) {
      State const& state = *get_state(state_nr);

      // Epsilon transitions
      {
        IntervalSet<StateNrType> const state_nrs_next = state.get_epsilon_transitions();
        for (size_t j = 0; j < state_nrs_next.size(); ++j) {
          Interval<StateNrType> const state_nrs_next_interval = state_nrs_next.get_by_index(j);
          for (StateNrType state_nr_next = state_nrs_next_interval.get_lower(); state_nr_next <= state_nrs_next_interval.get_upper(); ++state_nr_next) {
            std::cerr << state_nr << " -- E --> " << state_nr_next << std::endl;
          }
        }
      }

      // Symbol transitions
      {
        IntervalSet<SymbolType> const symbols = state.get_symbols();
        for (size_t j = 0; j < symbols.size(); ++j) {
          Interval<SymbolType> const symbols_interval = symbols.get_by_index(j);
          for (SymbolType symbol = symbols_interval.get_lower(); symbol <= symbols_interval.get_upper(); ++symbol) {
            StateNrType const state_nr_next = state.get_symbol_transition(Symbol(symbol));
            std::cerr << state_nr << " -- SYM[" << symbol << "] --> " << state_nr_next << std::endl;
          }
        }
      }
    }
  }
}

}  // namespace pmt::util::smct