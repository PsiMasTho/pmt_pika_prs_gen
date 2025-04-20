#include "pmt/util/smct/state_machine_determinizer.hpp"

#include "pmt/base/bitset_converter.hpp"

namespace pmt::util::smct {
using namespace pmt::base;

void StateMachineDeterminizer::determinize(StateMachine& state_machine_) {
  if (state_machine_.get_state_count() == 0) {
    return;
  }

  std::unordered_map<State::StateNrType, IntervalSet<State::StateNrType>> e_closure_cache;
  std::unordered_map<State::StateNrType, IntervalSet<State::StateNrType>> new_to_old;
  std::unordered_map<Bitset, State::StateNrType> old_to_new;
  std::vector<State::StateNrType> pending;
  StateMachine result;

  auto const take = [&pending]() {
    State::StateNrType ret = pending.back();
    pending.pop_back();
    return ret;
  };

  auto const push_and_visit = [&](IntervalSet<State::StateNrType> state_nr_set_) -> State::StateNrType {
    Bitset const state_nr_set_bitset = BitsetConverter::from_interval_set(state_nr_set_);
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
    State& state_cur = result.get_or_create_state(state_nr_cur);
    IntervalSet<State::StateNrType> const& state_nr_set_cur = new_to_old.find(state_nr_cur)->second;

    // Set up the accepts
    for (size_t i = 0; i < state_nr_set_cur.size(); ++i) {
      Interval<State::StateNrType> const& interval = state_nr_set_cur.get_by_index(i);
      for (State::StateNrType state_nr_old = interval.get_lower(); state_nr_old <= interval.get_upper(); ++state_nr_old) {
        State const& state_old = state_machine_.get_or_create_state(state_nr_old);
        Bitset const& accepts_old = state_old.get_accepts();
        if (accepts_old.empty()) {
          continue;
        }

        Bitset& accepts_cur = state_cur.get_accepts();
        accepts_cur.resize(accepts_old.size(), false);
        accepts_cur.inplace_or(accepts_old);
      }
    }

    // Set up the transitions
    IntervalSet<Symbol::UnderlyingType> const symbols = get_symbols(state_machine_, state_nr_set_cur);
    for (size_t i = 0; i < symbols.size(); ++i) {
      Interval<Symbol::UnderlyingType> const& interval = symbols.get_by_index(i);
      for (Symbol::UnderlyingType symbol = interval.get_lower(); symbol <= interval.get_upper(); ++symbol) {
        IntervalSet<State::StateNrType> state_nr_set_next = get_e_closure(state_machine_, get_moves(state_machine_, state_nr_set_cur, Symbol(symbol)), e_closure_cache);
        state_cur.add_symbol_transition(Symbol(symbol), push_and_visit(std::move(state_nr_set_next)));
      }
    }
  }

  state_machine_ = std::move(result);
}

auto StateMachineDeterminizer::get_e_closure(StateMachine const& state_machine_, State::StateNrType state_nr_from_, std::unordered_map<State::StateNrType, IntervalSet<State::StateNrType>>& cache_) -> IntervalSet<State::StateNrType> {
  IntervalSet<State::StateNrType> ret;
  std::vector<State::StateNrType> pending;
  pending.push_back(state_nr_from_);
  ret.insert(Interval<State::StateNrType>(state_nr_from_));

  while (!pending.empty()) {
    State::StateNrType const state_nr_cur = pending.back();
    pending.pop_back();

    if (auto const itr = cache_.find(state_nr_cur); itr != cache_.end()) {
      // Already cached, inplace_or
      IntervalSet<State::StateNrType> const& cached = itr->second;
      ret.inplace_or(cached);
      continue;
    }

    IntervalSet<State::StateNrType> const& epsilon_transitions = state_machine_.get_state(state_nr_cur)->get_epsilon_transitions();
    for (size_t i = 0; i < epsilon_transitions.size(); ++i) {
      Interval<State::StateNrType> const& interval = epsilon_transitions.get_by_index(i);
      for (State::StateNrType state_nr_next = interval.get_lower(); state_nr_next <= interval.get_upper(); ++state_nr_next) {
        if (!ret.contains(state_nr_next)) {
          ret.insert(Interval<State::StateNrType>(state_nr_next));
          pending.push_back(state_nr_next);
        }
      }
    }
  }

  // Cache the result
  cache_.insert_or_assign(state_nr_from_, ret);

  return ret;
}

auto StateMachineDeterminizer::get_e_closure(StateMachine const& state_machine_, IntervalSet<State::StateNrType> const& state_nrs_from_, std::unordered_map<State::StateNrType, IntervalSet<State::StateNrType>>& cache_) -> IntervalSet<State::StateNrType> {
  IntervalSet<State::StateNrType> ret;

  for (size_t i = 0; i < state_nrs_from_.size(); ++i) {
    Interval<State::StateNrType> const& interval = state_nrs_from_.get_by_index(i);
    for (State::StateNrType state_nr_from = interval.get_lower(); state_nr_from <= interval.get_upper(); ++state_nr_from) {
      ret.inplace_or(get_e_closure(state_machine_, state_nr_from, cache_));
    }
  }

  return ret;
}

auto StateMachineDeterminizer::get_moves(StateMachine const& state_machine_, IntervalSet<State::StateNrType> const& state_nrs_from_, Symbol symbol_) -> IntervalSet<State::StateNrType> {
  IntervalSet<State::StateNrType> ret;

  for (size_t i = 0; i < state_nrs_from_.size(); ++i) {
    Interval<State::StateNrType> const& interval = state_nrs_from_.get_by_index(i);
    for (State::StateNrType state_nr_from = interval.get_lower(); state_nr_from <= interval.get_upper(); ++state_nr_from) {
      State const& state_from = *state_machine_.get_state(state_nr_from);
      State::StateNrType const state_nr_next = state_from.get_symbol_transition(symbol_);
      if (state_nr_next != State::StateNrSink) {
        ret.insert(Interval<State::StateNrType>(state_nr_next));
      }
    }
  }
  return ret;
}

auto StateMachineDeterminizer::get_symbols(StateMachine const& state_machine_, IntervalSet<State::StateNrType> const& state_nrs_from_) -> IntervalSet<Symbol::UnderlyingType> {
  IntervalSet<Symbol::UnderlyingType> ret;

  for (size_t i = 0; i < state_nrs_from_.size(); ++i) {
    Interval<State::StateNrType> const& interval = state_nrs_from_.get_by_index(i);
    for (State::StateNrType state_nr_from = interval.get_lower(); state_nr_from <= interval.get_upper(); ++state_nr_from) {
      State const& state_from = *state_machine_.get_state(state_nr_from);
      IntervalSet<Symbol::UnderlyingType> const symbols = state_from.get_symbols();
      ret.inplace_or(symbols);
    }
  }

  return ret;
}

}  // namespace pmt::util::smct