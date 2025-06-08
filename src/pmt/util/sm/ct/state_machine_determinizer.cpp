#include "pmt/util/sm/ct/state_machine_determinizer.hpp"

#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/sm/ct/symbol.hpp"

namespace pmt::util::sm::ct {
using namespace pmt::base;

namespace {
class Locals {
 public:
  std::unordered_map<StateNrType, IntervalSet<StateNrType>> _e_closure_cache;
  std::unordered_map<StateNrType, IntervalSet<StateNrType>> _new_to_old;
  std::unordered_map<IntervalSet<StateNrType>, StateNrType> _old_to_new;
  std::vector<StateNrType> _pending;
  StateMachine _output_state_machine;
};

auto push_and_visit(Locals& locals_, IntervalSet<StateNrType> state_nr_set_) -> StateNrType {
 if (auto const itr = locals_._old_to_new.find(state_nr_set_); itr != locals_._old_to_new.end()) {
   return itr->second;
 }

 StateNrType const state_nr = locals_._output_state_machine.get_unused_state_nr();
 locals_._output_state_machine.get_or_create_state(state_nr);
 locals_._new_to_old.insert_or_assign(state_nr, state_nr_set_);
 locals_._old_to_new.insert_or_assign(state_nr_set_, state_nr);
 locals_._pending.push_back(state_nr);
 return state_nr;
}

auto take(Locals& locals_) -> StateNrType {
 StateNrType ret = locals_._pending.back();
 locals_._pending.pop_back();
 return ret;
}

auto get_e_closure(StateMachineDeterminizer::Args args_, Locals& locals_, StateNrType state_nr_from_) -> IntervalSet<StateNrType> {
  IntervalSet<StateNrType> ret;
  std::vector<StateNrType> pending;
  pending.push_back(state_nr_from_);
  ret.insert(Interval<StateNrType>(state_nr_from_));

  while (!pending.empty()) {
    StateNrType const state_nr_cur = pending.back();
    pending.pop_back();

    if (auto const itr = locals_._e_closure_cache.find(state_nr_cur); itr != locals_._e_closure_cache.end()) {
      // Already cached, inplace_or
      IntervalSet<StateNrType> const& cached = itr->second;
      ret.inplace_or(cached);
      continue;
    }

    args_._state_machine.get_state(state_nr_cur)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) {
      if (!ret.contains(state_nr_next_)) {
        ret.insert(Interval<StateNrType>(state_nr_next_));
        pending.push_back(state_nr_next_);
      }
    });
  }

  // Cache the result
  locals_._e_closure_cache.insert_or_assign(state_nr_from_, ret);

  return ret;
}

auto get_e_closure(StateMachineDeterminizer::Args args_, Locals& locals_, IntervalSet<StateNrType> const& state_nrs_from_) -> IntervalSet<StateNrType> {
  IntervalSet<StateNrType> ret;

  state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) { ret.inplace_or(get_e_closure(args_, locals_, state_nr_from_)); });

  return ret;
}

auto get_moves(StateMachineDeterminizer::Args args_, IntervalSet<StateNrType> const& state_nrs_from_, Symbol symbol_) -> IntervalSet<StateNrType> {
  IntervalSet<StateNrType> ret;

  state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) {
    State const& state_from = *args_._state_machine.get_state(state_nr_from_);
    StateNrType const state_nr_next = state_from.get_symbol_transition(symbol_);
    if (state_nr_next != StateNrSink) {
      ret.insert(Interval<StateNrType>(state_nr_next));
    }
  });

  return ret;
}

auto get_kinds(StateMachineDeterminizer::Args args_, IntervalSet<StateNrType> const& state_nrs_from_) -> IntervalSet<SymbolKindType> {
  IntervalSet<SymbolKindType> ret;

  state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) {
    State const& state_from = *args_._state_machine.get_state(state_nr_from_);
    IntervalSet<SymbolKindType> const kinds = state_from.get_symbol_kinds();
    ret.inplace_or(kinds);
  });

  return ret;
}

auto get_symbols(StateMachineDeterminizer::Args args_, SymbolKindType kind_, IntervalSet<StateNrType> const& state_nrs_from_) -> IntervalSet<SymbolValueType> {
  IntervalSet<SymbolValueType> ret;

  state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) {
    State const& state_from = *args_._state_machine.get_state(state_nr_from_);
    ret.inplace_or(state_from.get_symbol_values(kind_));
  });

  return ret;
}

}

void StateMachineDeterminizer::determinize(Args args_) {
  if (args_._state_machine.get_state_count() == 0) {
    return;
  }
 
  Locals locals;

  push_and_visit(locals, get_e_closure(args_, locals, args_._state_nr_from));

  while (!locals._pending.empty()) {
   StateNrType const state_nr_cur = take(locals);
   State& state_cur = locals._output_state_machine.get_or_create_state(state_nr_cur);
   IntervalSet<StateNrType> const& state_nr_set_cur = locals._new_to_old.find(state_nr_cur)->second;

   // Set up the accepts
   state_nr_set_cur.for_each_key([&](StateNrType state_nr_old_) {
     State const& state_old = *args_._state_machine.get_state(state_nr_old_);
     state_cur.get_accepts().inplace_or(state_old.get_accepts());
   });

   // Set up the transitions
   get_kinds(args_, state_nr_set_cur).for_each_key([&](SymbolKindType kind_) {
    get_symbols(args_, kind_, state_nr_set_cur).for_each_key([&](SymbolValueType symbol_) {
     state_cur.add_symbol_transition(Symbol(kind_, symbol_), push_and_visit(locals, get_e_closure(args_, locals, get_moves(args_, state_nr_set_cur, Symbol(kind_, symbol_)))));
    });
   });
  }

  args_._state_machine = std::move(locals._output_state_machine);
}

}  // namespace pmt::util::smct