#include "pmt/util/parsect/state_machine_minimizer.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"
#include "pmt/util/parsect/alphabet_limits.hpp"

namespace pmt::util::parsect {

void StateMachineMinimizer::minimize(StateMachine& state_machine_) {
  // Determine the alphabet limits
  AlphabetLimits alphabet_limits(state_machine_);

  // Initialize partitions
  std::unordered_map<pmt::base::Bitset, pmt::base::Bitset> by_accepts;

  for (State::StateNrType state_nr = 0; state_nr < state_machine_.size(); ++state_nr) {
    if (state_nr == state_machine_.get_state_nr_sink()) {
      by_accepts.insert_or_assign(pmt::base::Bitset(state_machine_.size(), false), Bitset(state_machine_.size(), false));
      continue;
    }

    State const& state = *state_machine_.get_state(state_nr);
    auto itr = by_accepts.find(state._accepts);
    if (itr == by_accepts.end()) {
      itr = by_accepts.insert_or_assign(state._accepts, Bitset(state_machine_.size(), false)).first;
    }
    itr->second.set(state_nr, true);
  }

  std::unordered_set<pmt::base::Bitset> p;

  for (auto const& [accepts, state_nrs] : by_accepts) {
    p.insert(state_nrs);
  }

  std::unordered_set<pmt::base::Bitset> w = p;

  while (!w.empty()) {
    pmt::base::Bitset a = *w.begin();
    w.erase(w.begin());

    for (size_t i = 0; i < StateTraits::SymbolKindCount; ++i) {
      std::optional<std::pair<Symbol::ValueType, Symbol::ValueType>> const& limits = alphabet_limits.get_limit(i);
      if (!limits.has_value()) {
        continue;
      }

      for (Symbol::ValueType j = limits->first; j <= limits->second; ++j) {
        pmt::base::Bitset x(state_machine_.size(), false);
        for (size_t k = 0; k < state_machine_.size(); ++k) {
          State::StateNrType const state_nr_next = state_machine_.get_state_nr_next(k, i, j);
          if (a.get(state_nr_next)) {
            x.set(k, true);
          }
        }

        std::unordered_set<pmt::base::Bitset> p_new;
        for (pmt::base::Bitset const& y : p) {
          pmt::base::Bitset const x_intersect_y = x.clone_and(y);
          pmt::base::Bitset const x_diff_y = y.clone_asymmetric_difference(x);

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

  // Construct the minimized fa
  std::unordered_map<State::StateNrType, pmt::base::Bitset const*> state_nr_to_equiv_class;

  for (pmt::base::Bitset const& equivalence_class : p) {
    for (State::StateNrType state_nr = 0; state_nr < state_machine_.size(); ++state_nr) {
      if (equivalence_class.get(state_nr)) {
        state_nr_to_equiv_class.insert_or_assign(state_nr, &equivalence_class);
      }
    }
  }

  std::vector<pmt::base::Bitset const*> pending;
  std::unordered_map<pmt::base::Bitset const*, State::StateNrType> visited;
  StateMachine result;

  auto const take = [&pending]() {
    pmt::base::Bitset const* ret = pending.back();
    pending.pop_back();
    return ret;
  };

  auto const push_and_visit = [&](pmt::base::Bitset const* item_) -> State::StateNrType {
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
    pmt::base::Bitset const* bitset_cur = take();
    State::StateNrType const state_nr_cur = visited.find(bitset_cur)->second;
    State& state_cur = result._states.find(state_nr_cur)->second;

    // Set up the accepts
    std::unordered_set<State::StateNrType> const state_nrs_cur = pmt::base::BitsetConverter::to_unordered_set<State::StateNrType>(*bitset_cur);
    for (State::StateNrType const state_nr_old : state_nrs_cur) {
      pmt::base::Bitset const& accepts_old = state_machine_.find(state_nr_old)->second._accepts;
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

          pmt::base::Bitset const& bitset_next_old = *state_nr_to_equiv_class.find(state_nr_next_old)->second;
          State::StateNrType const state_nr_next_new = push_and_visit(&bitset_next_old);
          state_cur._transitions._symbol_transitions[i].insert_or_assign(j, state_nr_next_new);
        }
      }
    }
  }

  state_machine_ = std::move(result);
}

}  // namespace pmt::util::parsect