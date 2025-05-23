#include "pmt/util/smct/state_machine_pruner.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/interval_map.hpp"
#include "pmt/util/smct/state_machine.hpp"

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

StateMachinePruner::StateMachinePruner(StateMachine const& state_machine_)
: _input_state_machine(state_machine_)
, _state_nr_from(StateNrStart) {
}

auto StateMachinePruner::set_state_nr_from(pmt::util::smrt::StateNrType state_nr_from_) && -> StateMachinePruner&& {
 _state_nr_from = state_nr_from_;
 return std::move(*this);
}

auto StateMachinePruner::enable_renumbering(pmt::util::smrt::StateNrType state_nr_from_new_) && -> StateMachinePruner&& {
 _state_nr_from_new = state_nr_from_new_;
 return std::move(*this);
}

auto StateMachinePruner::prune() && -> StateMachine {
 push_and_visit(_state_nr_from);

 while (!_pending.empty()) {
  StateNrType const state_nr_old = take();
  StateNrType const state_nr_new = *_visited.find(state_nr_old);
  State const& state_old = *_input_state_machine.get_state(state_nr_old);
  State& state_new = _output_state_machine.get_or_create_state(state_nr_new);

  copy_accepts(state_old, state_new);
  follow_and_copy_epsilon_transitions(state_old, state_new);
  follow_and_copy_symbol_transitions(state_old, state_new);
 }

 return std::move(_output_state_machine);
}

auto StateMachinePruner::push_and_visit(StateNrType state_nr_) -> StateNrType {
 if (StateNrType const* ptr = _visited.find(state_nr_); ptr != nullptr) {
   return *ptr;
 }

 StateNrType const state_nr_new = _state_nr_from_new.value_or(state_nr_);
 _state_nr_from_new = _state_nr_from_new.has_value() ? std::make_optional(state_nr_new + 1) : std::nullopt;

 _pending.push_back(state_nr_);
 _visited.insert(Interval<StateNrType>(state_nr_), state_nr_new);
 return state_nr_new;
}

auto StateMachinePruner::take() -> pmt::util::smrt::StateNrType {
 StateNrType const ret = _pending.back();
 _pending.pop_back();
 return ret;
}

void StateMachinePruner::copy_accepts(pmt::util::smct::State const& state_old_, pmt::util::smct::State& state_new_) {
  state_new_.get_accepts() = state_old_.get_accepts();
}

void StateMachinePruner::follow_and_copy_epsilon_transitions(pmt::util::smct::State const& state_old_, pmt::util::smct::State& state_new_) {
 state_old_.get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_old_) {
  StateNrType const state_nr_next_new = push_and_visit(state_nr_next_old_);
  state_new_.add_epsilon_transition(state_nr_next_new);
 });
}

void StateMachinePruner::follow_and_copy_symbol_transitions(pmt::util::smct::State const& state_old_, pmt::util::smct::State& state_new_) {
 state_old_.get_symbol_transitions().for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolType> const& interval_) {
  state_new_.add_symbol_transition(interval_, push_and_visit(state_nr_next_old_));
 });
}

}  // namespace pmt::util::smct