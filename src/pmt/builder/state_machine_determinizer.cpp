#include "pmt/builder/state_machine_determinizer.hpp"

#include "pmt/container/interval_set.hpp"

#include <vector>

namespace pmt::builder {
using namespace pmt::rt;
using namespace pmt::container;

namespace {

class Locals {
public:
 std::unordered_map<IntervalSet<StateNrType>, StateNrType> _old_to_new;
 std::vector<IntervalSet<StateNrType>> _new_to_old;
 std::vector<StateNrType> _pending;
 StateMachine _output_state_machine;
};

auto push_and_visit(Locals& locals_, IntervalSet<StateNrType> const& state_nr_set_) -> StateNrType {
 if (auto const itr = locals_._old_to_new.find(state_nr_set_); itr != locals_._old_to_new.end()) {
  return itr->second;
 }

 StateNrType const state_nr = locals_._new_to_old.size();
 locals_._output_state_machine.get_or_create_state(state_nr);
 locals_._old_to_new.insert_or_assign(state_nr_set_, state_nr);
 locals_._new_to_old.push_back(state_nr_set_);
 locals_._pending.push_back(state_nr);
 return state_nr;
}

auto take(Locals& locals_) -> StateNrType {
 StateNrType ret = locals_._pending.back();
 locals_._pending.pop_back();
 return ret;
}

auto get_e_closure(StateMachine const& input_state_machine_, Locals& locals_, StateNrType state_nr_from_) -> IntervalSet<StateNrType> {
 IntervalSet<StateNrType> ret;
 std::vector<StateNrType> pending;
 pending.push_back(state_nr_from_);
 ret.insert(Interval<StateNrType>(state_nr_from_));

 while (!pending.empty()) {
  StateNrType const state_nr_cur = pending.back();
  pending.pop_back();

  input_state_machine_.get_state(state_nr_cur)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) {
   if (!ret.contains(state_nr_next_)) {
    ret.insert(Interval<StateNrType>(state_nr_next_));
    pending.push_back(state_nr_next_);
   }
  });
 }

 return ret;
}

auto get_e_closure(StateMachine const& input_state_machine_, Locals& locals_, IntervalSet<StateNrType> const& state_nrs_from_) -> IntervalSet<StateNrType> {
 IntervalSet<StateNrType> ret;

 state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) { ret.inplace_or(get_e_closure(input_state_machine_, locals_, state_nr_from_)); });

 return ret;
}

auto get_moves(StateMachine const& input_state_machine_, IntervalSet<StateNrType> const& state_nrs_from_, SymbolType symbol_) -> IntervalSet<StateNrType> {
 IntervalSet<StateNrType> ret;

 state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) {
  State const& state_from = *input_state_machine_.get_state(state_nr_from_);
  StateNrType const state_nr_next = state_from.get_symbol_transition(symbol_);
  if (state_nr_next != StateNrInvalid) {
   ret.insert(Interval<StateNrType>(state_nr_next));
  }
 });

 return ret;
}

auto get_symbols(StateMachine const& input_state_machine_, IntervalSet<StateNrType> const& state_nrs_from_) -> IntervalSet<SymbolType> {
 IntervalSet<SymbolType> ret;

 state_nrs_from_.for_each_key([&](StateNrType state_nr_from_) { ret.inplace_or(input_state_machine_.get_state(state_nr_from_)->get_symbols()); });

 return ret;
}

}  // namespace

auto StateMachineDeterminizer::determinize(StateMachine const& input_state_machine_, pmt::rt::StateNrType state_nr_from_) -> StateMachine {
 if (input_state_machine_.get_state_count() == 0) {
  return StateMachine();
 }

 Locals locals;

 push_and_visit(locals, get_e_closure(input_state_machine_, locals, state_nr_from_));

 while (!locals._pending.empty()) {
  StateNrType const state_nr_cur = take(locals);

  // Set up the accepts
  locals._new_to_old[state_nr_cur].for_each_key([&](StateNrType state_nr_old_) { locals._output_state_machine.get_state(state_nr_cur)->add_final_ids(input_state_machine_.get_state(state_nr_old_)->get_final_ids()); });

  // Set up the transitions
  get_symbols(input_state_machine_, locals._new_to_old[state_nr_cur]).for_each_key([&](SymbolType symbol_) {
   StateNrType const state_nr_next = push_and_visit(locals, get_e_closure(input_state_machine_, locals, get_moves(input_state_machine_, locals._new_to_old[state_nr_cur], symbol_)));
   locals._output_state_machine.get_state(state_nr_cur)->add_symbol_transition(symbol_, state_nr_next);
  });
 }

 return std::move(locals._output_state_machine);
}

}  // namespace pmt::builder