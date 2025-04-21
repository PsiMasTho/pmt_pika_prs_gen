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

  IntervalSet<StateNrType> const state_nrs = state_machine_.get_state_nrs();
  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<StateNrType> const& interval = state_nrs.get_by_index(i);
    for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *state_machine_.get_state(state_nr);
      IntervalSet<SymbolType> const symbols = state.get_symbols();
      alphabet.inplace_or(symbols);
    }
  }

  return alphabet;
}

auto get_initial_partitions(StateMachine const& state_machine_) -> std::unordered_set<IntervalSet<StateNrType>> {
  // Initialize partitions where a partition is a set of states that have the same combination of accepts

  // <accepts, state_nrs>
  // Using IntervalSet<size_t> instead of Bitset for accepts because the bitsets
  // may be different sizes and that can mess with the comparison and hashing
  std::unordered_map<IntervalSet<size_t>, IntervalSet<StateNrType>> by_accepts;

  IntervalSet<StateNrType> const state_nrs = state_machine_.get_state_nrs();
  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<StateNrType> const& interval = state_nrs.get_by_index(i);
    for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *state_machine_.get_state(state_nr);
      IntervalSet<size_t> accepts = BitsetConverter::to_interval_set(state.get_accepts());
      auto itr = by_accepts.find(accepts);
      if (itr == by_accepts.end()) {
        itr = by_accepts.insert_or_assign(std::move(accepts), IntervalSet<StateNrType>()).first;
      }
      itr->second.insert(Interval<StateNrType>(state_nr));
    }
  }

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

    for (size_t i = 0; i < alphabet_.size(); ++i) {
      Interval<SymbolType> const& interval = alphabet_.get_by_index(i);
      for (SymbolType c = interval.get_lower(); c <= interval.get_upper(); ++c) {
        IntervalSet<StateNrType> x;

        // Add the sink state
        x.insert(Interval<StateNrType>(StateNrSink));

        for (size_t k = 0; k < state_nrs.size(); ++k) {
          Interval<StateNrType> const& interval = state_nrs.get_by_index(k);
          for (StateNrType l = interval.get_lower(); l <= interval.get_upper(); ++l) {
            // Note: state_nr_next will be the sink if there is no transition, so
            // that is handled here
            StateNrType const state_nr_next = state_machine_.get_state(l)->get_symbol_transition(Symbol(c));
            if (a.contains(state_nr_next)) {
              x.insert(Interval<StateNrType>(l));
            }
          }

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
    }
  }

  return initial_partitions_;
}

auto get_state_nr_to_equivalence_partitions_mapping(StateMachine const& state_machine_, std::unordered_set<IntervalSet<StateNrType>> const& equivalence_partitions_) -> std::unordered_map<StateNrType, IntervalSet<StateNrType> const*> {
  std::unordered_map<StateNrType, IntervalSet<StateNrType> const*> ret;
  ret.reserve(state_machine_.get_state_count());

  IntervalSet<StateNrType> const state_nrs = state_machine_.get_state_nrs();

  for (IntervalSet<StateNrType> const& equivalence_partition : equivalence_partitions_) {
    for (size_t i = 0; i < state_nrs.size(); ++i) {
      Interval<StateNrType> const& interval = state_nrs.get_by_index(i);
      for (StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
        if (equivalence_partition.contains(state_nr)) {
          ret.insert_or_assign(state_nr, &equivalence_partition);
        }
      }
    }

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

  push_and_visit(equivalence_partition_mapping_.find(0)->second);

  while (!pending.empty()) {
    IntervalSet<StateNrType> const* equivalence_partition_cur = take();
    StateNrType const state_nr_cur = visited.find(equivalence_partition_cur)->second;
    State& state_cur = *result.get_state(state_nr_cur);

    // Set up the accepts
    for (size_t i = 0; i < equivalence_partition_cur->size(); ++i) {
      Interval<StateNrType> const& state_nr_old_interval = equivalence_partition_cur->get_by_index(i);
      for (StateNrType state_nr_old = state_nr_old_interval.get_lower(); state_nr_old <= state_nr_old_interval.get_upper(); ++state_nr_old) {
        State const& state_old = *state_machine_.get_state(state_nr_old);
        Bitset const& accepts_old = state_old.get_accepts();

        // Set up the accepts
        if (accepts_old.popcnt() != 0) {
          state_cur.get_accepts().resize(accepts_old.size(), false);  // todo: fix bitset impl so that ORing resizes when needed by itself
          state_cur.get_accepts().inplace_or(accepts_old);
        }

        // Set up the transitions
        IntervalSet<SymbolType> const symbols = state_old.get_symbols();
        for (size_t i = 0; i < symbols.size(); ++i) {
          Interval<SymbolType> const& symbol_interval = symbols.get_by_index(i);
          for (SymbolType c = symbol_interval.get_lower(); c <= symbol_interval.get_upper(); ++c) {
            StateNrType const state_nr_next_old = state_old.get_symbol_transition(Symbol(c));

            if (state_nr_next_old == StateNrSink) {
              continue;
            }

            IntervalSet<StateNrType> const& equivalence_partition_next_old = *equivalence_partition_mapping_.find(state_nr_next_old)->second;
            StateNrType const state_nr_next_new = push_and_visit(&equivalence_partition_next_old);
            state_cur.add_symbol_transition(Symbol(c), state_nr_next_new);
          }
        }
      }
    }
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