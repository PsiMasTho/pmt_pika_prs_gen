#include "pmt/parser/builder/state_machine_tables.hpp"

#include <cassert>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::sm;

namespace {}

StateMachineTables::StateMachineTables(StateMachine state_machine_)
 : _state_machine(std::move(state_machine_)) {
 init_cache();
}

auto StateMachineTables::get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType {
 State const* state = _state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(symbol_) : StateNrInvalid;
}

auto StateMachineTables::get_state_accept_count(pmt::sm::StateNrType state_nr_) const -> size_t {
 assert(state_nr_ < _state_accepts_cache.size());
 return _state_accepts_cache[state_nr_].size();
}

auto StateMachineTables::get_state_accept_at(pmt::sm::StateNrType state_nr_, size_t index_) const -> pmt::sm::AcceptsIndexType {
 assert(state_nr_ < _state_accepts_cache.size());
 assert(index_ < _state_accepts_cache[state_nr_].size());
 return _state_accepts_cache[state_nr_][index_];
}

auto StateMachineTables::get_state_machine() const -> StateMachine const& {
 return _state_machine;
}

void StateMachineTables::init_cache() {
 _state_accepts_cache.clear();
 _state_accepts_cache.resize(_state_machine.get_state_count());

 for (StateNrType state_nr = 0; state_nr < _state_machine.get_state_count(); ++state_nr) {
  State const* state = _state_machine.get_state(state_nr);
  if (state) {
   state->get_accepts().for_each_interval([&](Interval<AcceptsIndexType> interval_) {
    for (AcceptsIndexType accept = interval_.get_lower(); accept <= interval_.get_upper(); ++accept) {
     _state_accepts_cache[state_nr].push_back(accept);
    }
   });
  }
 }
}

}  // namespace pmt::parser::builder