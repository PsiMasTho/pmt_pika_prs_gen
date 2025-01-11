#include "pmt/util/parsect/state_machine_pruner.hpp"

#include "pmt/base/dynamic_bitset.hpp"

namespace pmt::util::parsect {
using namespace pmt::base;

void StateMachinePruner::prune(StateMachine& state_machine_, State::StateNrType state_nr_from_, State::StateNrType state_nr_from_new_, bool renumber_) {
  std::vector<State::StateNrType> pending;
  std::unordered_map<State::StateNrType, State::StateNrType> visited;

  auto const push_and_visit = [&pending, &visited, &state_nr_from_new_, &renumber_](State::StateNrType state_nr_) -> State::StateNrType {
    if (auto const itr = visited.find(state_nr_); itr != visited.end()) {
      return itr->second;
    }

    State::StateNrType const state_nr_new = renumber_ ? state_nr_from_new_++ : state_nr_;

    pending.push_back(state_nr_);
    visited.insert_or_assign(state_nr_, state_nr_new);
    return state_nr_new;
  };

  push_and_visit(state_nr_from_);

  StateMachine res;

  while (!pending.empty()) {
    State::StateNrType const state_nr_old = pending.back();
    pending.pop_back();
    State::StateNrType const state_nr_new = visited.find(state_nr_old)->second;
    State const& state_old = *state_machine_.get_state(state_nr_old);
    State& state_new = res.get_or_create_state(state_nr_new);

    state_new.get_accepts() = state_old.get_accepts();

    for (State::StateNrType const state_nr_next_old : state_old.get_epsilon_transitions()) {
      state_new.add_epsilon_transition(push_and_visit(state_nr_next_old));
    }

    for (Symbol::KindType const kind : state_old.get_symbol_kinds()) {
      for (Symbol const& symbol : state_old.get_symbols(kind)) {
        State::StateNrType const state_nr_next_old = state_old.get_symbol_transition(symbol);
        state_new.add_symbol_transition(symbol, push_and_visit(state_nr_next_old));
      }
    }
  }

  state_machine_ = std::move(res);
}

}  // namespace pmt::util::parsect