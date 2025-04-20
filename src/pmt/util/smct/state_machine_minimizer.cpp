#include "pmt/util/smct/state_machine_minimizer.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"
#include "pmt/util/smct/state_machine.hpp"

namespace pmt::util::smct {
using namespace pmt::base;

namespace {
auto get_alphabet(StateMachine const& state_machine_) -> IntervalSet<Symbol::UnderlyingType> {
  IntervalSet<Symbol::UnderlyingType> alphabet;

  IntervalSet<State::StateNrType> const state_nrs = state_machine_.get_state_nrs();
  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<State::StateNrType> const& interval = state_nrs.get_by_index(i);
    for (State::StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *state_machine_.get_state(state_nr);
      IntervalSet<Symbol::UnderlyingType> const symbols = state.get_symbols();
      alphabet.inplace_or(symbols);
    }
  }

  return alphabet;
}

auto get_partitions(StateMachine const& state_machine_) -> std::unordered_set<IntervalSet<State::StateNrType>> {
  // Initialize partitions where a partition is a set of states that have the same combination of accepts
  std::unordered_map<Bitset, IntervalSet<State::StateNrType>> by_accepts;

  IntervalSet<State::StateNrType> const state_nrs = state_machine_.get_state_nrs();
  for (size_t i = 0; i < state_nrs.size(); ++i) {
    Interval<State::StateNrType> const& interval = state_nrs.get_by_index(i);
    for (State::StateNrType state_nr = interval.get_lower(); state_nr <= interval.get_upper(); ++state_nr) {
      State const& state = *state_machine_.get_state(state_nr);
      auto itr = by_accepts.find(state.get_accepts());
      if (itr == by_accepts.end()) {
        itr = by_accepts.insert_or_assign(state.get_accepts(), IntervalSet<State::StateNrType>()).first;
      }
      itr->second.insert(Interval<State::StateNrType>(state_nr));
    }
  }

  std::unordered_set<IntervalSet<State::StateNrType>> ret;

  for (auto& [accepts, state_nrs] : by_accepts) {
    ret.insert(std::move(state_nrs));
  }

  return ret;
}
}  // namespace

void StateMachineMinimizer::minimize(StateMachine& state_machine_) {
  IntervalSet<Symbol::UnderlyingType> alphabet = get_alphabet(state_machine_);

  std::unordered_set<IntervalSet<State::StateNrType>> p = get_partitions(state_machine_);
  std::unordered_set<IntervalSet<State::StateNrType>> w = p;

  IntervalSet<State::StateNrType> const state_nrs = state_machine_.get_state_nrs();

  while (!w.empty()) {
    IntervalSet<State::StateNrType> a = *w.begin();
    w.erase(w.begin());

    for (size_t i = 0; i < alphabet.size(); ++i) {
      Interval<Symbol::UnderlyingType> const& interval = alphabet.get_by_index(i);
      for (Symbol::UnderlyingType j = interval.get_lower(); j <= interval.get_upper(); ++j) {
        IntervalSet<State::StateNrType> x;
        for (size_t k = 0; k < state_nrs.size(); ++k) {
          Interval<State::StateNrType> const& interval = state_nrs.get_by_index(k);
          for (State::StateNrType l = interval.get_lower(); l <= interval.get_upper(); ++l) {
            State::StateNrType const state_nr_next = state_machine_.get_state(l)->get_symbol_transition(Symbol(j));
            if (a.contains(state_nr_next)) {
              x.insert(Interval<State::StateNrType>(l));
            }
          }

          std::unordered_set<IntervalSet<State::StateNrType>> p_new;
          for (IntervalSet<State::StateNrType> const& y : p) {
            IntervalSet<State::StateNrType> const x_intersect_y = x.clone_and(y);
            IntervalSet<State::StateNrType> const x_diff_y = y.clone_asymmetric_difference(x);

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

          p = std::move(p_new);
        }
      }
    }
  }

  // Construct the minimized fa
  std::unordered_map<State::StateNrType, Bitset const*> state_nr_to_equiv_class;

  for (Bitset const& equivalence_class : p) {
    for (State::StateNrType state_nr = 0; state_nr < state_machine_.size(); ++state_nr) {
      if (equivalence_class.get(state_nr)) {
        state_nr_to_equiv_class.insert_or_assign(state_nr, &equivalence_class);
      }
    }
  }

  std::vector<Bitset const*> pending;
  std::unordered_map<Bitset const*, State::StateNrType> visited;
  StateMachine result;

  auto const take = [&pending]() {
    Bitset const* ret = pending.back();
    pending.pop_back();
    return ret;
  };

  auto const push_and_visit = [&](Bitset const* item_) -> State::StateNrType {
    auto itr = visited.find(item_);
    if (itr != visited.end()) {
      return itr->second;
    } else {
      itr = visited.insert_or_assign(item_, result.get_unused_state_nr()).first;
    }

    pending.push_back(item_);
    visited.insert_or_assign(item_, itr->second);
    result._states[itr->second];
    return itr->second;
  };

  push_and_visit(state_nr_to_equiv_class.find(0)->second);

  while (!pending.empty()) {
    Bitset const* bitset_cur = take();
    State::StateNrType const state_nr_cur = visited.find(bitset_cur)->second;
    State& state_cur = result._states.find(state_nr_cur)->second;

    // Set up the accepts
    std::unordered_set<State::StateNrType> const state_nrs_cur = BitsetConverter::to_unordered_set<State::StateNrType>(*bitset_cur);
    for (State::StateNrType const state_nr_old : state_nrs_cur) {
      Bitset const& accepts_old = state_machine_.find(state_nr_old)->second._accepts;
      // Set up the accepts
      if (accepts_old.popcnt() != 0) {
        state_cur._accepts.resize(accepts_old.size(), false);
        state_cur._accepts.inplace_or(accepts_old);
      }

      // Set up the transitions
      for (size_t i = 0; i < StateTraits::SymbolKindCount; ++i) {
        std::optional<std::pair<Symbol::ValueType, Symbol::ValueType>> const& limits = alphabet_limits.get_limit(i);
        if (!limits.has_value()) {
          continue;
        }

        for (Symbol::ValueType j = limits->first; j <= limits->second; ++j) {
          State::StateNrType const state_nr_next_old = state_machine_.get_state_nr_next(state_nr_old, i, j);

          if (state_nr_next_old == state_machine_.get_state_nr_sink()) {
            continue;
          }

          Bitset const& bitset_next_old = *state_nr_to_equiv_class.find(state_nr_next_old)->second;
          State::StateNrType const state_nr_next_new = push_and_visit(&bitset_next_old);
          state_cur._transitions._symbol_transitions[i].insert_or_assign(j, state_nr_next_new);
        }
      }
    }
  }

  state_machine_ = std::move(result);
}

}  // namespace pmt::util::smct