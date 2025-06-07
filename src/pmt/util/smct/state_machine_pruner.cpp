#include "pmt/util/smct/state_machine_pruner.hpp"

#include "pmt/util/smct/state_machine.hpp"

#include <vector>
#include <unordered_map>

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

namespace {
class Locals {
 public:
  std::unordered_map<StateNrType, StateNrType> _old_to_new;
  std::vector<StateNrType> _pending;
};

auto push_and_visit(StateMachinePruner::Args& args_, Locals& locals_, StateNrType state_nr_) {
 if (auto const itr = locals_._old_to_new.find(state_nr_); itr != locals_._old_to_new.end()) {
   return;
 }

 StateNrType const state_nr_new = args_._state_nr_from_new.value_or(state_nr_);
 args_._state_nr_from_new = args_._state_nr_from_new.has_value() ? std::make_optional(state_nr_new + 1) : std::nullopt;

 locals_._pending.push_back(state_nr_);
 locals_._old_to_new.emplace(state_nr_, state_nr_new);
 return;
}

auto take(Locals& locals_) -> StateNrType {
 StateNrType const ret = locals_._pending.back();
 locals_._pending.pop_back();
 return ret;
}

void follow_epsilon_transitions(StateMachinePruner::Args& args_, Locals& locals_, State const& state_old_) {
 state_old_.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_old_) {
  push_and_visit(args_, locals_, state_nr_next_old_);
 });
}

void follow_symbol_transitions(StateMachinePruner::Args& args_, Locals& locals_, State const& state_old_) {
 state_old_.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
  state_old_.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolValueType> const& interval_) {
   push_and_visit(args_, locals_, state_nr_next_old_);
  });
 });
}

void copy_epsilon_transitions(Locals& locals_, StateMachine& state_machine_new_, State const& state_old_, State& state_new_) {
 state_old_.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_old_) {
  state_new_.add_epsilon_transition(locals_._old_to_new.find(state_nr_next_old_)->second);
 });
}

void copy_symbol_transitions(Locals& locals_, StateMachine& state_machine_new_, State const& state_old_, State& state_new_) {
 state_old_.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
  state_old_.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolValueType> const& interval_) {
   state_new_.add_symbol_transition(kind_, interval_, locals_._old_to_new.find(state_nr_next_old_)->second);
  });
 });
}

}

void StateMachinePruner::prune(Args args_) {
 Locals locals;
 push_and_visit(args_, locals, args_._state_nr_from);

 while (!locals._pending.empty()) {
  StateNrType const state_nr_old = take(locals);
  State const* state_old = args_._state_machine.get_state(state_nr_old);

  follow_epsilon_transitions(args_, locals, *state_old);
  follow_symbol_transitions(args_, locals, *state_old);
 }

 // Remove states that were not visited
 args_._state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  if (locals._old_to_new.find(state_nr_) != locals._old_to_new.end()) {
   return;
  }
  args_._state_machine.remove_state(state_nr_);
 });

 // Renumber states if necessary
 if (args_._state_nr_from_new.has_value()) {
  StateMachine state_machine_new;
  for (auto const& [state_nr_old, state_nr_new] : locals._old_to_new) {
   State const* state_old = args_._state_machine.get_state(state_nr_old);
   State& state_new = state_machine_new.get_or_create_state(state_nr_new);

   copy_epsilon_transitions(locals, state_machine_new, *state_old, state_new);
   copy_symbol_transitions(locals, state_machine_new, *state_old, state_new);
   state_new.get_accepts() = state_old->get_accepts();
  }

  // Replace the old state machine with the new one
  args_._state_machine = std::move(state_machine_new);
 }
}

}  // namespace pmt::util::smct