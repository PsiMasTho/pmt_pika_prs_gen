#include "pmt/sm/state_machine_pruner.hpp"

#include "pmt/sm/state_machine.hpp"

#include <unordered_map>
#include <vector>

namespace pmt::sm {
namespace {
class Locals {
public:
 std::unordered_map<StateNrType, StateNrType> _old_to_new;
 std::vector<StateNrType> _pending;
};

}  // namespace

void push_and_visit(StateMachinePruner::Args& args_, Locals& locals_, StateNrType state_nr_) {
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

void follow_epsilon_transitions(StateMachinePruner::Args& args_, Locals& locals_, State& state_old_) {
 pmt::base::IntervalSet<StateNrType> to_remove;

 state_old_.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_old_) {
  if (args_._input_state_machine.get_state(state_nr_next_old_) == nullptr) {
   to_remove.insert(pmt::base::Interval(state_nr_next_old_));
  } else {
   push_and_visit(args_, locals_, state_nr_next_old_);
  }
 });

 to_remove.for_each_key([&](StateNrType state_nr_to_remove_) { state_old_.remove_epsilon_transition(state_nr_to_remove_); });
}

void follow_symbol_transitions(StateMachinePruner::Args& args_, Locals& locals_, State& state_old_) {
 std::vector<pmt::base::Interval<SymbolType>> to_remove;

 state_old_.get_symbol_transitions().for_each_interval([&](StateNrType state_nr_next_old_, pmt::base::Interval<SymbolType> const& symbol_interval_) {
  if (args_._input_state_machine.get_state(state_nr_next_old_) == nullptr) {
   to_remove.push_back(symbol_interval_);
  } else {
   push_and_visit(args_, locals_, state_nr_next_old_);
  }
 });

 while (!to_remove.empty()) {
  pmt::base::Interval<SymbolType> const symbol_interval = to_remove.back();
  state_old_.remove_symbol_transitions(symbol_interval);
  to_remove.pop_back();
 }
}

void copy_epsilon_transitions(Locals& locals_, StateMachine& output_state_machine_, State const& state_old_, State& state_new_) {
 state_old_.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_old_) { state_new_.add_epsilon_transition(locals_._old_to_new.find(state_nr_next_old_)->second); });
}

void copy_symbol_transitions(Locals& locals_, StateMachine& output_state_machine_, State const& state_old_, State& state_new_) {
 state_old_.get_symbol_transitions().for_each_interval([&](StateNrType state_nr_next_old_, pmt::base::Interval<SymbolType> const& symbol_interval_) { state_new_.add_symbol_transitions(symbol_interval_, locals_._old_to_new.find(state_nr_next_old_)->second); });
}

auto StateMachinePruner::prune(Args args_) -> StateMachine {
 Locals locals;
 push_and_visit(args_, locals, args_._state_nr_from);

 while (!locals._pending.empty()) {
  StateNrType const state_nr_old = take(locals);
  State& state_old = *args_._input_state_machine.get_state(state_nr_old);

  follow_epsilon_transitions(args_, locals, state_old);
  follow_symbol_transitions(args_, locals, state_old);
 }

 // Remove states that were not visited
 for (StateNrType const state_nr : args_._input_state_machine.get_state_nrs()) {
  if (locals._old_to_new.contains(state_nr)) {
   continue;
  }
  args_._input_state_machine.remove_state(state_nr);
 }

 // Renumber states if necessary
 if (args_._state_nr_from_new.has_value()) {
  StateMachine output_state_machine;
  for (auto const& [state_nr_old, state_nr_new] : locals._old_to_new) {
   State const* state_old = args_._input_state_machine.get_state(state_nr_old);
   State& state_new = output_state_machine.get_or_create_state(state_nr_new);

   copy_epsilon_transitions(locals, output_state_machine, *state_old, state_new);
   copy_symbol_transitions(locals, output_state_machine, *state_old, state_new);
   state_new.add_accepts(state_old->get_accepts());
  }

  // Replace the old state machine with the new one
  return std::move(output_state_machine);
 }

 return std::move(args_._input_state_machine);
}

}  // namespace pmt::sm