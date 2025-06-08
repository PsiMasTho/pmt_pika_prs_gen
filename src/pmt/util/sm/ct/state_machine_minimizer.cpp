#include "pmt/util/sm/ct/state_machine_minimizer.hpp"

#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <unordered_set>

namespace pmt::util::sm::ct {
using namespace pmt::base;

namespace {
auto get_alphabet(StateMachine const& state_machine_) -> std::unordered_set<Symbol> {
  std::unordered_set<Symbol> ret;

  state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State const& state = *state_machine_.get_state(state_nr_);
    state.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
     state.get_symbol_values(kind_).for_each_key([&](SymbolValueType value_) {
        ret.insert(Symbol(kind_, value_));
      });
    });
  });

  return ret;
}

auto get_initial_partitions(StateMachine const& state_machine_) -> std::unordered_set<IntervalSet<StateNrType>> {
  // Initialize partitions where a partition is a set of states that have the same combination of accepts

  // <accepts, state_nrs>
  std::unordered_map<IntervalSet<AcceptsIndexType>, IntervalSet<StateNrType>> by_accepts;

  state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State const& state = *state_machine_.get_state(state_nr_);
    IntervalSet<AcceptsIndexType> const& accepts = state.get_accepts();
    auto itr = by_accepts.find(accepts);
    if (itr == by_accepts.end()) {
      itr = by_accepts.insert_or_assign(accepts, IntervalSet<StateNrType>()).first;
    }
    itr->second.insert(Interval<StateNrType>(state_nr_));
  });

  // Insert the sink state, it has no accepts
  by_accepts[IntervalSet<AcceptsIndexType>{}].insert(Interval<StateNrType>(StateNrSink));

  std::unordered_set<IntervalSet<StateNrType>> ret;

  for (auto& [accepts, state_nrs] : by_accepts) {
    ret.insert(std::move(state_nrs));
  }

  return ret;
}

auto get_equivalence_partitions(StateMachine const& state_machine_, std::unordered_set<Symbol> const& alphabet_, std::unordered_set<IntervalSet<StateNrType>> initial_partitions_) -> std::unordered_set<IntervalSet<StateNrType>> {
  std::unordered_set<IntervalSet<StateNrType>> w = initial_partitions_;

  IntervalSet<StateNrType> const state_nrs = state_machine_.get_state_nrs();

  while (!w.empty()) {
    IntervalSet<StateNrType> a = *w.begin();
    w.erase(w.begin());

    for (Symbol const& c : alphabet_) {
      IntervalSet<StateNrType> x;

      // Add the sink state
      x.insert(Interval<StateNrType>(StateNrSink));

      state_nrs.for_each_key([&](StateNrType state_nr_from_) {
        // Note: state_nr_to will be the sink if there is no transition, so
        // that is handled here
        StateNrType const state_nr_to = state_machine_.get_state(state_nr_from_)->get_symbol_transition(c);
        if (a.contains(state_nr_to)) {
          x.insert(Interval<StateNrType>(state_nr_from_));
        }
      });

      std::unordered_set<IntervalSet<StateNrType>> p_new;
      for (IntervalSet<StateNrType> const& y : initial_partitions_) {
        IntervalSet<StateNrType> const x_intersect_y = x.clone_and(y);
        IntervalSet<StateNrType> const x_diff_y = y.clone_asymmetric_difference(x);

        size_t const popcnt_x_intersect_y = x_intersect_y.popcnt();
        size_t const popcnt_x_diff_y = x_diff_y.popcnt();

        if (popcnt_x_intersect_y == 0 || popcnt_x_diff_y == 0) {
          p_new.insert(y);
          continue;
        }

        p_new.insert(x_intersect_y);
        p_new.insert(x_diff_y);

        auto const itr = w.find(y);
        if (itr != w.end()) {
          w.erase(itr);
          w.insert(x_intersect_y);
          w.insert(x_diff_y);
        } else {
          (popcnt_x_intersect_y <= popcnt_x_diff_y) ? w.insert(x_intersect_y) : w.insert(x_diff_y);
        }
      }

      initial_partitions_ = std::move(p_new);
    }
  }

  return initial_partitions_;
}

auto get_state_nr_to_equivalence_partitions_mapping(StateMachine const& state_machine_, std::unordered_set<IntervalSet<StateNrType>> const& equivalence_partitions_) -> std::unordered_map<StateNrType, IntervalSet<StateNrType> const*> {
  std::unordered_map<StateNrType, IntervalSet<StateNrType> const*> ret;
  ret.reserve(state_machine_.get_state_count());

  IntervalSet<StateNrType> const state_nrs = state_machine_.get_state_nrs();

  for (IntervalSet<StateNrType> const& equivalence_partition : equivalence_partitions_) {
    equivalence_partition.for_each_key([&](StateNrType state_nr_) {
      if (equivalence_partition.contains(state_nr_)) {
        ret.insert_or_assign(state_nr_, &equivalence_partition);
      }
    });

    // Insert the sink state
    if (equivalence_partition.contains(StateNrSink)) {
      ret.insert_or_assign(StateNrSink, &equivalence_partition);
    }
  }

  return ret;
}

auto rebuild_minimized_state_machine(StateMachine& state_machine_, std::unordered_map<StateNrType, IntervalSet<StateNrType> const*> const& equivalence_partition_mapping_) -> StateMachine {
  std::vector<IntervalSet<StateNrType> const*> pending;
  std::unordered_map<IntervalSet<StateNrType> const*, StateNrType> visited;
  StateMachine result;

  auto const take = [&pending]() {
    IntervalSet<StateNrType> const* ret = pending.back();
    pending.pop_back();
    return ret;
  };

  auto const push_and_visit = [&](IntervalSet<StateNrType> const* item_) -> StateNrType {
    auto itr = visited.find(item_);
    if (itr != visited.end()) {
      return itr->second;
    } else {
      itr = visited.insert_or_assign(item_, result.get_unused_state_nr()).first;
    }

    pending.push_back(item_);
    visited.insert_or_assign(item_, itr->second);
    result.get_or_create_state(itr->second);
    return itr->second;
  };

  if (auto const itr = equivalence_partition_mapping_.find(StateNrStart); itr != equivalence_partition_mapping_.end()) {
    push_and_visit(itr->second);
  } 

  while (!pending.empty()) {
    IntervalSet<StateNrType> const* equivalence_partition_cur = take();
    StateNrType const state_nr_cur = visited.find(equivalence_partition_cur)->second;
    State& state_cur = *result.get_state(state_nr_cur);

    equivalence_partition_cur->for_each_key([&](StateNrType state_nr_old_) {
      State const& state_old = *state_machine_.get_state(state_nr_old_);
      state_cur.get_accepts().inplace_or(state_old.get_accepts());

      // Set up the transitions
      state_old.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
       state_old.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolValueType> const& interval_) {
        if (state_nr_next_old_ == StateNrSink) {
          return;
        }
        IntervalSet<StateNrType> const& equivalence_partition_next_old = *equivalence_partition_mapping_.find(state_nr_next_old_)->second;
        StateNrType const state_nr_next_new = push_and_visit(&equivalence_partition_next_old);
        state_cur.add_symbol_transition(kind_, interval_, state_nr_next_new);
       });
      });
    });
  }

  return result;
}

}  // namespace

void StateMachineMinimizer::minimize(StateMachine& state_machine_) {
 std::unordered_set<IntervalSet<StateNrType>> const p = get_equivalence_partitions(state_machine_, get_alphabet(state_machine_), get_initial_partitions(state_machine_));
 std::unordered_map<StateNrType, IntervalSet<StateNrType> const*> const state_nr_to_equiv_partition = get_state_nr_to_equivalence_partitions_mapping(state_machine_, p);
 state_machine_ = rebuild_minimized_state_machine(state_machine_, state_nr_to_equiv_partition);
}

}  // namespace pmt::util::smct