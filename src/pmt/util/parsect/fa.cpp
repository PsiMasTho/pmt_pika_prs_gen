#include "pmt/util/parsect/fa.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/util/parsect/fa_sink_wrapper.hpp"
#include "pmt/util/parsert/generic_tables_base.hpp"

#include <climits>
#include <vector>

namespace pmt::util::parsect {
using namespace pmt::base;
using namespace pmt::util::parsert;

void Fa::prune(StateNrType state_nr_from_, StateNrType state_nr_from_new_, bool renumber_) {
  std::vector<StateNrType> pending;
  std::unordered_map<StateNrType, StateNrType> visited;

  auto const push_and_visit = [&pending, &visited, &state_nr_from_new_, &renumber_](StateNrType state_nr_) -> StateNrType {
    if (auto const itr = visited.find(state_nr_); itr != visited.end()) {
      return itr->second;
    }

    Fa::StateNrType const state_nr_new = renumber_ ? state_nr_from_new_++ : state_nr_;

    pending.push_back(state_nr_);
    visited.insert_or_assign(state_nr_, state_nr_new);
    return state_nr_new;
  };

  auto const take = [&pending]() {
    StateNrType ret = pending.back();
    pending.pop_back();
    return ret;
  };

  push_and_visit(state_nr_from_);

  Fa res;

  while (!pending.empty()) {
    StateNrType const state_nr_old = take();
    StateNrType const state_nr_new = visited.find(state_nr_old)->second;
    State const& state_old = _states.find(state_nr_old)->second;
    State& state_new = res._states[state_nr_new];
    state_new._accepts = state_old._accepts;

    for (auto const& [symbol, state_nr_next_old] : state_old._transitions._symbol_transitions) {
      StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
      state_new._transitions._symbol_transitions.insert_or_assign(symbol, state_nr_next_new);
    }

    for (StateNrType state_nr_next_old : state_old._transitions._epsilon_transitions) {
      StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old);
      state_new._transitions._epsilon_transitions.insert(state_nr_next_new);
    }
  }

  *this = std::move(res);
}

void Fa::determinize() {
  subset();
}

void Fa::minimize() {
  hopcroft();
}

auto Fa::get_unused_state_nr() const -> StateNrType {
  StateNrType ret = 0;
  while (_states.contains(ret)) {
    ++ret;
  }
  return ret;
}

void Fa::subset() {
  std::unordered_map<StateNrType, std::unordered_set<StateNrType>> e_closure_cache;
  std::unordered_map<StateNrType, std::unordered_set<StateNrType>> new_to_old;
  std::unordered_map<DynamicBitset, StateNrType> old_to_new;
  std::vector<StateNrType> pending;
  Fa result;

  auto const take = [&pending]() {
    StateNrType ret = pending.back();
    pending.pop_back();
    return ret;
  };

  auto const push_and_visit = [&](std::unordered_set<StateNrType> state_nr_set_) -> StateNrType {
    DynamicBitset const state_nr_set_bitset = DynamicBitsetConverter::from_unordered_set(state_nr_set_, _states.size());
    if (auto const itr = old_to_new.find(state_nr_set_bitset); itr != old_to_new.end()) {
      return itr->second;
    }

    StateNrType const state_nr = result.get_unused_state_nr();
    result._states[state_nr];
    new_to_old.insert_or_assign(state_nr, std::move(state_nr_set_));
    old_to_new.insert_or_assign(state_nr_set_bitset, state_nr);
    pending.push_back(state_nr);
    return state_nr;
  };

  push_and_visit(get_e_closure(0, e_closure_cache));

  while (!pending.empty()) {
    StateNrType const state_nr_cur = take();
    State& state_cur = result._states.find(state_nr_cur)->second;
    std::unordered_set<StateNrType> const& state_nr_set_cur = new_to_old.find(state_nr_cur)->second;

    // Set up the accepts
    for (StateNrType const state_nr_old : state_nr_set_cur) {
      State const& state_old = _states.find(state_nr_old)->second;
      if (state_old._accepts.empty()) {
        continue;
      }

      state_cur._accepts.resize(state_old._accepts.size(), false);
      state_cur._accepts.inplace_or(_states.find(state_nr_old)->second._accepts);
    }

    // Set up the transitions
    std::unordered_set<SymbolType> const symbols = get_symbols(state_nr_set_cur);
    for (SymbolType symbol : symbols) {
      std::unordered_set<StateNrType> state_nr_set_next = get_e_closure(get_moves(state_nr_set_cur, symbol), e_closure_cache);
      StateNrType const state_nr_next = push_and_visit(std::move(state_nr_set_next));
      state_cur._transitions._symbol_transitions.insert_or_assign(symbol, state_nr_next);
    }
  }

  *this = std::move(result);
}

auto Fa::get_e_closure(StateNrType state_nr_from_, std::unordered_map<StateNrType, std::unordered_set<StateNrType>>& cache_) const -> std::unordered_set<StateNrType> {
  std::unordered_set<StateNrType> ret;
  std::vector<StateNrType> pending;
  pending.push_back(state_nr_from_);
  ret.insert(state_nr_from_);

  while (!pending.empty()) {
    StateNrType const state_nr_cur = pending.back();
    pending.pop_back();

    if (auto const itr = cache_.find(state_nr_cur); itr != cache_.end()) {
      ret.insert(itr->second.begin(), itr->second.end());
      continue;
    }

    State const& state_cur = _states.find(state_nr_cur)->second;
    for (StateNrType state_nr_next : state_cur._transitions._epsilon_transitions) {
      if (ret.insert(state_nr_next).second) {
        pending.push_back(state_nr_next);
      }
    }
  }

  // Cache the result
  cache_.insert_or_assign(state_nr_from_, ret);

  return ret;
}

auto Fa::get_e_closure(std::unordered_set<StateNrType> const& state_nrs_from_, std::unordered_map<StateNrType, std::unordered_set<StateNrType>>& cache_) const -> std::unordered_set<StateNrType> {
  std::unordered_set<StateNrType> ret;

  for (StateNrType const state_nr_from : state_nrs_from_) {
    ret.merge(get_e_closure(state_nr_from, cache_));
  }

  return ret;
}

auto Fa::get_moves(std::unordered_set<StateNrType> const& state_nrs_from_, SymbolType symbol_) const -> std::unordered_set<StateNrType> {
  std::unordered_set<StateNrType> ret;

  for (StateNrType const state_nr_from : state_nrs_from_) {
    State const& state_from = _states.find(state_nr_from)->second;
    auto const itr = state_from._transitions._symbol_transitions.find(symbol_);
    if (itr != state_from._transitions._symbol_transitions.end()) {
      ret.insert(itr->second);
    }
  }

  return ret;
}

auto Fa::get_symbols(std::unordered_set<StateNrType> const& state_nrs_from_) const -> std::unordered_set<SymbolType> {
  std::unordered_set<SymbolType> ret;

  for (StateNrType const state_nr_from : state_nrs_from_) {
    State const& state_from = _states.find(state_nr_from)->second;
    for (auto const& [symbol, state_nr_next] : state_from._transitions._symbol_transitions) {
      ret.insert(symbol);
    }
  }

  return ret;
}

void Fa::hopcroft() {
  // Create a wrapper to mimic having a sink state
  FaSinkWrapper fa_with_sink(*this);

  // Initialize partitions
  std::unordered_map<DynamicBitset, DynamicBitset> by_accepts;

  for (Fa::StateNrType state_nr = 0; state_nr < fa_with_sink.get_size(); ++state_nr) {
    if (state_nr == fa_with_sink.get_state_nr_sink()) {
      by_accepts.insert_or_assign(DynamicBitset(fa_with_sink.get_size(), false), DynamicBitset(fa_with_sink.get_size(), false));
      continue;
    }

    Fa::State const& state = _states.find(state_nr)->second;
    auto itr = by_accepts.find(state._accepts);
    if (itr == by_accepts.end()) {
      itr = by_accepts.insert_or_assign(state._accepts, DynamicBitset(fa_with_sink.get_size(), false)).first;
    }
    itr->second.set(state_nr, true);
  }

  std::unordered_set<DynamicBitset> p;

  for (auto const& [accepts, state_nrs] : by_accepts) {
    p.insert(state_nrs);
  }

  std::unordered_set<DynamicBitset> w = p;

  while (!w.empty()) {
    DynamicBitset a = *w.begin();
    w.erase(w.begin());

    for (Fa::SymbolType c = 0; c <= GenericTablesBase::SYMBOL_EOI; ++c) {
      DynamicBitset x(fa_with_sink.get_size(), false);
      for (size_t i = 0; i < fa_with_sink.get_size(); ++i) {
        Fa::StateNrType const state_nr_next = fa_with_sink.get_state_nr_next(i, c);
        if (a.get(state_nr_next)) {
          x.set(i, true);
        }
      }

      std::unordered_set<DynamicBitset> p_new;
      for (DynamicBitset const& y : p) {
        DynamicBitset const x_intersect_y = x.clone_and(y);
        DynamicBitset const x_diff_y = y.clone_asymmetric_difference(x);

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

  // Construct the minimized fa
  std::unordered_map<StateNrType, DynamicBitset const*> state_nr_to_equiv_class;

  for (DynamicBitset const& equivalence_class : p) {
    for (Fa::StateNrType state_nr = 0; state_nr < fa_with_sink.get_size(); ++state_nr) {
      if (equivalence_class.get(state_nr)) {
        state_nr_to_equiv_class.insert_or_assign(state_nr, &equivalence_class);
      }
    }
  }

  std::vector<DynamicBitset const*> pending;
  std::unordered_map<DynamicBitset const*, StateNrType> visited;
  Fa result;

  auto const take = [&pending]() {
    DynamicBitset const* ret = pending.back();
    pending.pop_back();
    return ret;
  };

  auto const push_and_visit = [&](DynamicBitset const* item_) -> StateNrType {
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
    DynamicBitset const* bitset_cur = take();
    StateNrType const state_nr_cur = visited.find(bitset_cur)->second;
    State& state_cur = result._states.find(state_nr_cur)->second;

    // Set up the accepts
    std::unordered_set<Fa::StateNrType> const state_nrs_cur = DynamicBitsetConverter::to_unordered_set<Fa::StateNrType>(*bitset_cur);
    for (Fa::StateNrType const state_nr_old : state_nrs_cur) {
      DynamicBitset const& accepts_old = _states.find(state_nr_old)->second._accepts;
      // Set up the accepts
      if (accepts_old.popcnt() != 0) {
        state_cur._accepts.resize(accepts_old.size(), false);
        state_cur._accepts.inplace_or(accepts_old);
      }

      // Set up the transitions
      for (Fa::SymbolType symbol = 0; symbol <= GenericTablesBase::SYMBOL_EOI; ++symbol) {
        Fa::StateNrType const state_nr_next_old = fa_with_sink.get_state_nr_next(state_nr_old, symbol);

        if (state_nr_next_old == fa_with_sink.get_state_nr_sink()) {
          continue;
        }

        DynamicBitset const& bitset_next_old = *state_nr_to_equiv_class.find(state_nr_next_old)->second;
        StateNrType const state_nr_next_new = push_and_visit(&bitset_next_old);
        state_cur._transitions._symbol_transitions.insert_or_assign(symbol, state_nr_next_new);
      }
    }
  }

  *this = std::move(result);
}

}  // namespace pmt::util::parsect