#include "pmt/util/smct/state_machine_pruner.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/interval_map.hpp"
#include "pmt/util/smct/state_machine.hpp"

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

void StateMachinePruner::prune(StateMachine& state_machine_, StateNrType state_nr_from_, StateNrType state_nr_from_new_, bool renumber_) {
  std::vector<StateNrType> pending;
  IntervalMap<StateNrType, StateNrType> visited;

  auto const push_and_visit = [&pending, &visited, &state_nr_from_new_, &renumber_](StateNrType state_nr_) -> StateNrType {
    if (StateNrType const* ptr = visited.find(state_nr_); ptr != nullptr) {
      return *ptr;
    }

    StateNrType const state_nr_new = renumber_ ? state_nr_from_new_++ : state_nr_;

    pending.push_back(state_nr_);
    visited.insert(Interval<StateNrType>(state_nr_), state_nr_new);
    return state_nr_new;
  };

  push_and_visit(state_nr_from_);

  StateMachine res;

  while (!pending.empty()) {
    StateNrType const state_nr_old = pending.back();
    pending.pop_back();
    StateNrType const state_nr_new = *visited.find(state_nr_old);
    State const& state_old = *state_machine_.get_state(state_nr_old);
    State& state_new = res.get_or_create_state(state_nr_new);

    state_new.get_accepts() = state_old.get_accepts();

    {
      IntervalSet<StateNrType> const& epsilon_transitions = state_old.get_epsilon_transitions();
      for (size_t i = 0; i < epsilon_transitions.size(); ++i) {
        Interval<StateNrType> const& interval = epsilon_transitions.get_by_index(i);
        for (StateNrType state_nr_next_old = interval.get_lower(); state_nr_next_old <= interval.get_upper(); ++state_nr_next_old) {
          state_new.add_epsilon_transition(push_and_visit(state_nr_next_old));
        }
      }
    }

    {
      IntervalSet<SymbolType> const symbols = state_old.get_symbols();
      for (size_t i = 0; i < symbols.size(); ++i) {
        Interval<SymbolType> const& interval = symbols.get_by_index(i);
        for (SymbolType symbol = interval.get_lower(); symbol <= interval.get_upper(); ++symbol) {
          StateNrType const state_nr_next_old = state_old.get_symbol_transition(Symbol(symbol));
          state_new.add_symbol_transition(Symbol(symbol), push_and_visit(state_nr_next_old));
        }
      }
    }
  }

  state_machine_ = std::move(res);
}

}  // namespace pmt::util::smct