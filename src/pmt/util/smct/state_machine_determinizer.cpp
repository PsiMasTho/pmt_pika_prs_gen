#include "pmt/util/smct/state_machine_determinizer.hpp"

#include "pmt/util/smct/state_machine.hpp"

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

StateMachineDeterminizer::StateMachineDeterminizer(StateMachine const& state_machine_)
: _input_state_machine(state_machine_){}

auto StateMachineDeterminizer::set_state_nr_from(pmt::util::smrt::StateNrType state_nr_from_) && -> StateMachineDeterminizer&& {
 _state_nr_from = state_nr_from_;
 return std::move(*this);
}

auto StateMachineDeterminizer::determinize() && -> StateMachine {
  if (_input_state_machine.get_state_count() == 0) {
   return std::move(_output_state_machine);
  }

  push_and_visit(get_e_closure(_state_nr_from));

  while (!_pending.empty()) {
   StateNrType const state_nr_cur = take();
   State& state_cur = _output_state_machine.get_or_create_state(state_nr_cur);
   IntervalSet<StateNrType> const& state_nr_set_cur = _new_to_old.find(state_nr_cur)->second;

   // Set up the accepts
   state_nr_set_cur.for_each_key([&](StateNrType state_nr_old_) {
     State const& state_old = *_input_state_machine.get_state(state_nr_old_);
     Bitset const& accepts_old = state_old.get_accepts();
     if (accepts_old.empty()) {
       return;
     }
     Bitset& accepts_cur = state_cur.get_accepts();
     accepts_cur.resize(accepts_old.size(), false);
     accepts_cur.inplace_or(accepts_old);
   });

   // Set up the transitions
   get_symbols(state_nr_set_cur).for_each_key([&](SymbolType symbol_) { state_cur.add_symbol_transition(Symbol(symbol_), push_and_visit(get_e_closure(get_moves(state_nr_set_cur, Symbol(symbol_))))); });
  }

  return std::move(_output_state_machine);
}

auto StateMachineDeterminizer::push_and_visit(pmt::base::IntervalSet<pmt::util::smrt::StateNrType> state_nr_set_) -> pmt::util::smrt::StateNrType {
 if (auto const itr = _old_to_new.find(state_nr_set_); itr != _old_to_new.end()) {
   return itr->second;
 }

 StateNrType const state_nr = _output_state_machine.get_unused_state_nr();
 _output_state_machine.get_or_create_state(state_nr);
 _new_to_old.insert_or_assign(state_nr, state_nr_set_);
 _old_to_new.insert_or_assign(state_nr_set_, state_nr);
 _pending.push_back(state_nr);
 return state_nr;
}

auto StateMachineDeterminizer::take() -> pmt::util::smrt::StateNrType {
 StateNrType ret = _pending.back();
 _pending.pop_back();
 return ret;
}

auto StateMachineDeterminizer::get_e_closure(StateNrType state_nr_from_) -> IntervalSet<StateNrType> {
  IntervalSet<StateNrType> ret;
  std::vector<StateNrType> pending;
  pending.push_back(state_nr_from_);
  ret.insert(Interval<StateNrType>(state_nr_from_));

  while (!pending.empty()) {
    StateNrType const state_nr_cur = pending.back();
    pending.pop_back();

    if (auto const itr = _e_closure_cache.find(state_nr_cur); itr != _e_closure_cache.end()) {
      // Already cached, inplace_or
      IntervalSet<StateNrType> const& cached = itr->second;
      ret.inplace_or(cached);
      continue;
    }

    _input_state_machine.get_state(state_nr_cur)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) {
      if (!ret.contains(state_nr_next_)) {
        ret.insert(Interval<StateNrType>(state_nr_next_));
        pending.push_back(state_nr_next_);
      }
    });
  }

  // Cache the result
  _e_closure_cache.insert_or_assign(state_nr_from_, ret);

  return ret;
}

auto StateMachineDeterminizer::get_e_closure(IntervalSet<StateNrType> const& state_nrs_from_) -> IntervalSet<StateNrType> {
  IntervalSet<StateNrType> ret;

  state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) { ret.inplace_or(get_e_closure(state_nr_from_)); });

  return ret;
}

auto StateMachineDeterminizer::get_moves(IntervalSet<StateNrType> const& state_nrs_from_, Symbol symbol_) -> IntervalSet<StateNrType> {
  IntervalSet<StateNrType> ret;

  state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) {
    State const& state_from = *_input_state_machine.get_state(state_nr_from_);
    StateNrType const state_nr_next = state_from.get_symbol_transition(symbol_);
    if (state_nr_next != StateNrSink) {
      ret.insert(Interval<StateNrType>(state_nr_next));
    }
  });

  return ret;
}

auto StateMachineDeterminizer::get_symbols(IntervalSet<StateNrType> const& state_nrs_from_) -> IntervalSet<SymbolType> {
  IntervalSet<SymbolType> ret;

  state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) {
    State const& state_from = *_input_state_machine.get_state(state_nr_from_);
    IntervalSet<SymbolType> const symbols = state_from.get_symbols();
    ret.inplace_or(symbols);
  });

  return ret;
}

}  // namespace pmt::util::smct