#include "pmt/util/parse/fa.hpp"

#include "pmt/base/dynamic_bitset_converter.hpp"

#include <stack>

namespace pmt::util::parse {
using namespace pmt::base;

void Fa::prune(StateNrType state_nr_from_) {
  std::stack<StateNrType> stack;
  DynamicBitset visited(_states.size(), false);

  auto const push_and_visit = [&stack, &visited](StateNrType item_) {
    if (visited.set(item_, true)) {
      return;
    }
    stack.push(item_);
  };

  auto const take = [&stack]() {
    StateNrType ret = stack.top();
    stack.pop();
    return ret;
  };

  push_and_visit(state_nr_from_);

  while (!stack.empty()) {
    StateNrType const state_nr_cur = take();
    State& state = _states.find(state_nr_cur)->second;

    for (auto const& [symbol, state_nr_next] : state._transitions._symbol_transitions) {
      push_and_visit(state_nr_next);
    }

    for (StateNrType state_nr_next : state._transitions._epsilon_transitions) {
      push_and_visit(state_nr_next);
    }
  }

  // Remove unvisited states
  visited.inplace_not();
  std::unordered_set<size_t> const to_remove = DynamicBitsetConverter::to_unordered_set(visited);
  for (size_t state_nr : to_remove) {
    _states.erase(state_nr);
  }
}

void Fa::determinize() {
  subset();
}

void Fa::minimize() {
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
  std::unordered_map<StateNrType, std::unordered_set<StateNrType>> old_to_new;
  std::unordered_map<DynamicBitset, StateNrType> new_to_old;
  std::stack<StateNrType> stack;
  Fa result;

  auto const take = [&stack]() {
    StateNrType ret = stack.top();
    stack.pop();
    return ret;
  };

  {
    std::unordered_set<StateNrType> state_nr_set_initial = get_e_closure(0, e_closure_cache);
    StateNrType const state_nr_initial = result.get_unused_state_nr();
    result._states[state_nr_initial];
    old_to_new.insert_or_assign(state_nr_initial, state_nr_set_initial);
    new_to_old.insert_or_assign(DynamicBitsetConverter::from_unordered_set(state_nr_set_initial, _states.size()), state_nr_initial);
    stack.push(state_nr_initial);
  }

  while (!stack.empty()) {
    StateNrType const state_nr_cur = take();
    std::unordered_set<StateNrType> const state_nr_set_cur = old_to_new.find(state_nr_cur)->second;
    std::unordered_set<SymbolType> const symbols = get_symbols(state_nr_set_cur);

    for (SymbolType symbol : symbols) {
      std::unordered_set<StateNrType> U = get_e_closure(get_moves(state_nr_set_cur, symbol), e_closure_cache);
      DynamicBitset const U_bitset = DynamicBitsetConverter::from_unordered_set(U, _states.size());
      if (!new_to_old.contains(U_bitset)) {
        StateNrType const state_nr_U = result.get_unused_state_nr();
        result._states[state_nr_U];
        old_to_new.insert_or_assign(state_nr_U, U);
        new_to_old.insert_or_assign(U_bitset, state_nr_U);
        stack.push(state_nr_U);
      }
      StateNrType const state_nr_U = new_to_old.find(U_bitset)->second;
      result._states[state_nr_cur]._transitions._symbol_transitions.insert_or_assign(symbol, state_nr_U);
    }
  }

  *this = std::move(result);
}

auto Fa::get_e_closure(StateNrType state_nr_from_, std::unordered_map<StateNrType, std::unordered_set<StateNrType>>& cache_) const -> std::unordered_set<StateNrType> {
  std::unordered_set<StateNrType> ret;
  std::stack<StateNrType> stack;
  stack.push(state_nr_from_);
  ret.insert(state_nr_from_);

  while (!stack.empty()) {
    StateNrType const state_nr_cur = stack.top();
    stack.pop();

    if (auto const itr = cache_.find(state_nr_cur); itr != cache_.end()) {
      ret.insert(itr->second.begin(), itr->second.end());
      continue;
    }

    State const& state_cur = _states.find(state_nr_cur)->second;
    for (StateNrType state_nr_next : state_cur._transitions._epsilon_transitions) {
      if (ret.insert(state_nr_next).second) {
        stack.push(state_nr_next);
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

}  // namespace pmt::util::parse