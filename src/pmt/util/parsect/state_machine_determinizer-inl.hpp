// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/util/parsect/state_machine_determinizer.hpp"
#endif
// clang-format on

#include "pmt/base/dynamic_bitset_converter.hpp"

namespace pmt::util::parsect {
using namespace pmt::base;

template <IsStateTag TAG_>
void StateMachineDeterminizer<TAG_>::determinize(StateMachine<TAG_>& state_machine_) {
  std::unordered_map<State::StateNrType, std::unordered_set<State::StateNrType>> e_closure_cache;
  std::unordered_map<State::StateNrType, std::unordered_set<State::StateNrType>> new_to_old;
  std::unordered_map<DynamicBitset, State::StateNrType> old_to_new;
  std::vector<State::StateNrType> pending;
  StateMachine<TAG_> result;

  auto const take = [&pending]() {
    State::StateNrType ret = pending.back();
    pending.pop_back();
    return ret;
  };

  auto const push_and_visit = [&](std::unordered_set<State::StateNrType> state_nr_set_) -> State::StateNrType {
    DynamicBitset const state_nr_set_bitset = DynamicBitsetConverter::from_unordered_set(state_nr_set_, state_machine_.size());
    if (auto const itr = old_to_new.find(state_nr_set_bitset); itr != old_to_new.end()) {
      return itr->second;
    }

    State::StateNrType const state_nr = result.get_unused_state_nr();
    result.get_or_create_state(state_nr);
    new_to_old.insert_or_assign(state_nr, std::move(state_nr_set_));
    old_to_new.insert_or_assign(state_nr_set_bitset, state_nr);
    pending.push_back(state_nr);
    return state_nr;
  };

  push_and_visit(get_e_closure(state_machine_, 0, e_closure_cache));

  while (!pending.empty()) {
    State::StateNrType const state_nr_cur = take();
    State<TAG_>& state_cur = result.get_or_create_state(state_nr_cur);
    std::unordered_set<State::StateNrType> const& state_nr_set_cur = new_to_old.find(state_nr_cur)->second;

    // Set up the accepts
    for (State::StateNrType const state_nr_old : state_nr_set_cur) {
      State<TAG_> const& state_old = state_machine_.get_or_create_state(state_nr_old);
      if (state_old._accepts.empty()) {
        continue;
      }

      state_cur._accepts.resize(state_old._accepts.size(), false);
      state_cur._accepts.inplace_or(state_old._accepts);
    }

    // Set up the transitions
    std::unordered_set<Symbol> const symbols = get_symbols(state_machine_, state_nr_set_cur);
    for (Symbol const& symbol : symbols) {
      std::unordered_set<State::StateNrType> state_nr_set_next = get_e_closure(state_machine_, get_moves(state_machine_, state_nr_set_cur, symbol), e_closure_cache);
      State::StateNrType const state_nr_next = push_and_visit(std::move(state_nr_set_next));
      state_cur._symbol_transitions[symbol._symbol_kind].insert_or_assign(symbol, state_nr_next);
    }
  }

  state_machine_ = std::move(result);
}

template <IsStateTag TAG_>
auto StateMachineDeterminizer<TAG_>::get_e_closure(StateMachine<TAG_> const& state_machine_, State::StateNrType state_nr_from_, std::unordered_map<State::StateNrType, std::unordered_set<State::StateNrType>>& cache_) -> std::unordered_set<State::StateNrType> {
  std::unordered_set<State::StateNrType> ret;
  std::vector<State::StateNrType> pending;
  pending.push_back(state_nr_from_);
  ret.insert(state_nr_from_);

  while (!pending.empty()) {
    State::StateNrType const state_nr_cur = pending.back();
    pending.pop_back();

    if (auto const itr = cache_.find(state_nr_cur); itr != cache_.end()) {
      ret.insert(itr->second.begin(), itr->second.end());
      continue;
    }

    State<TAG_> const& state_cur = *state_machine_.get_state(state_nr_cur);
    for (State::StateNrType state_nr_next : state_cur._epsilon_transitions) {
      if (ret.insert(state_nr_next).second) {
        pending.push_back(state_nr_next);
      }
    }
  }

  // Cache the result
  cache_.insert_or_assign(state_nr_from_, ret);

  return ret;
}

template <IsStateTag TAG_>
auto StateMachineDeterminizer<TAG_>::get_e_closure(StateMachine<TAG_> const& state_machine_, std::unordered_set<State::StateNrType> const& state_nrs_from_, std::unordered_map<State::StateNrType, std::unordered_set<State::StateNrType>>& cache_) -> std::unordered_set<State::StateNrType> {
  std::unordered_set<State::StateNrType> ret;

  for (State::StateNrType const state_nr_from : state_nrs_from_) {
    ret.merge(get_e_closure(state_machine_, state_nr_from, cache_));
  }

  return ret;
}

template <IsStateTag TAG_>
auto StateMachineDeterminizer<TAG_>::get_moves(StateMachine<TAG_> const& state_machine_, std::unordered_set<State::StateNrType> const& state_nrs_from_, Symbol symbol_) -> std::unordered_set<State::StateNrType> {
  std::unordered_set<State::StateNrType> ret;

  for (State::StateNrType const state_nr_from : state_nrs_from_) {
    State<TAG_> const& state_from = *state_machine_.get_state(state_nr_from);
    auto const itr = state_from._symbol_transitions[symbol_._symbol_kind].find(symbol_);
    if (itr != state_from._symbol_transitions[symbol_._symbol_kind].end()) {
      ret.insert(itr->second);
    }
  }

  return ret;
}

template <IsStateTag TAG_>
auto StateMachineDeterminizer<TAG_>::get_symbols(StateMachine<TAG_> const& state_machine_, std::unordered_set<State::StateNrType> const& state_nrs_from_) -> std::unordered_set<Symbol> {
  std::unordered_set<Symbol> ret;

  for (State::StateNrType const state_nr_from : state_nrs_from_) {
    State<TAG_> const& state_from = *state_machine_.get_state(state_nr_from);
    for (auto const& map : state_from._symbol_transitions) {
      for (auto const& [symbol, state_nr_next] : map) {
        ret.insert(symbol);
      }
    }
  }

  return ret;
}

}  // namespace pmt::util::parsect