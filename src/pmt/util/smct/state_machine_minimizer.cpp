#include "pmt/util/smct/state_machine_minimizer.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"
#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

namespace {
auto get_alphabet(StateMachine const& state_machine_) -> IntervalSet<SymbolType> {
  IntervalSet<SymbolType> alphabet;

  state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State const& state = *state_machine_.get_state(state_nr_);
    IntervalSet<SymbolType> const symbols = state.get_symbols();
    alphabet.inplace_or(symbols);
  });

  return alphabet;
}

auto get_initial_partitions(StateMachine const& state_machine_) -> std::unordered_set<IntervalSet<StateNrType>> {
  // Initialize partitions where a partition is a set of states that have the same combination of accepts

  // <accepts, state_nrs>
  // Using IntervalSet<size_t> instead of Bitset for accepts because the bitsets
  // may be different sizes and that can mess with the comparison and hashing
  std::unordered_map<IntervalSet<size_t>, IntervalSet<StateNrType>> by_accepts;

  state_machine_.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
    State const& state = *state_machine_.get_state(state_nr_);
    IntervalSet<size_t> accepts = BitsetConverter::to_interval_set(state.get_accepts());
    auto itr = by_accepts.find(accepts);
    if (itr == by_accepts.end()) {
      itr = by_accepts.insert_or_assign(std::move(accepts), IntervalSet<StateNrType>()).first;
    }
    itr->second.insert(Interval<StateNrType>(state_nr_));
  });

  // Insert the sink state, it has no accepts
  by_accepts[IntervalSet<size_t>{}].insert(Interval<StateNrType>(StateNrSink));

  std::unordered_set<IntervalSet<StateNrType>> ret;

  for (auto& [accepts, state_nrs] : by_accepts) {
    ret.insert(std::move(state_nrs));
  }

  return ret;
}

auto get_equivalence_partitions(StateMachine const& state_machine_, IntervalSet<SymbolType> const& alphabet_, std::unordered_set<IntervalSet<StateNrType>> initial_partitions_) -> std::unordered_set<IntervalSet<StateNrType>> {
  std::unordered_set<IntervalSet<StateNrType>> w = initial_partitions_;

  IntervalSet<StateNrType> const state_nrs = state_machine_.get_state_nrs();

  while (!w.empty()) {
    IntervalSet<StateNrType> a = *w.begin();
    w.erase(w.begin());

    alphabet_.for_each_key([&](SymbolType c_) {
      IntervalSet<StateNrType> x;

      // Add the sink state
      x.insert(Interval<StateNrType>(StateNrSink));

      state_nrs.for_each_key([&](StateNrType state_nr_from_) {
        // Note: state_nr_to will be the sink if there is no transition, so
        // that is handled here
        StateNrType const state_nr_to = state_machine_.get_state(state_nr_from_)->get_symbol_transition(Symbol(c_));
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
    });
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
      Bitset const& accepts_old = state_old.get_accepts();

      // Set up the accepts
      if (accepts_old.popcnt() != 0) {
        state_cur.get_accepts().resize(accepts_old.size(), false);
        state_cur.get_accepts().inplace_or(accepts_old);
      }

      // Set up the transitions
      state_old.get_symbols().for_each_key([&](SymbolType symbol_) {
        StateNrType const state_nr_next_old = state_old.get_symbol_transition(Symbol(symbol_));
        if (state_nr_next_old == StateNrSink) {
          return;
        }
        IntervalSet<StateNrType> const& equivalence_partition_next_old = *equivalence_partition_mapping_.find(state_nr_next_old)->second;
        StateNrType const state_nr_next_new = push_and_visit(&equivalence_partition_next_old);
        state_cur.add_symbol_transition(Symbol(symbol_), state_nr_next_new);
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