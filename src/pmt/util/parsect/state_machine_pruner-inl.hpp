// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/util/parsect/state_machine_pruner.hpp"
#endif
// clang-format on

namespace pmt::util::parsect {

template <IsStateTag TAG_>
void StateMachinePruner::prune(StateMachine<TAG_>& state_machine_, State::StateNrType state_nr_from_, State::StateNrType state_nr_from_new_, bool renumber_) {
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

  StateMachine<TAG_> res;

  while (!pending.empty()) {
    State::StateNrType const state_nr_old = pending.back();
    pending.pop_back();
    State::StateNrType const state_nr_new = visited.find(state_nr_old)->second;
    State<TAG_> const& state_old = *state_machine_.get_state(state_nr_old);
    State<TAG_>& state_new = res.get_or_create_state(state_nr_new);
    state_new._accepts = state_old._accepts;

    for (State::StateNrType state_nr_next_old : state_old._epsilon_transitions) {
      State::StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
      state_new._epsilon_transitions.insert(state_nr_next_new);
    }

    for (size_t i = 0; i < state_old._symbol_transitions.size(); ++i) {
      for (auto const& [symbol, state_nr_next_old] : state_old._symbol_transitions[i]) {
        State::StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
        state_new._transitions[i].insert_or_assign(symbol, state_nr_next_new);
      }
    }
  }

  state_machine_ = std::move(res);
}

}  // namespace pmt::util::parsect