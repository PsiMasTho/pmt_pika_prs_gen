#include "pmt/builder/state_machine_tables.hpp"

#include "pmt/rt/primitives.hpp"

#include <cassert>

namespace pmt::builder {
using namespace pmt::rt;
using namespace pmt::container;

namespace {}

StateMachineTables::StateMachineTables(pmt::sm::StateMachine state_machine_)
 : _state_machine(std::move(state_machine_)) {
 init_cache();
}

auto StateMachineTables::get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType {
 pmt::sm::State const* state = _state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(symbol_).value_or(StateNrInvalid) : StateNrInvalid;
}

auto StateMachineTables::get_state_final_id_count(StateNrType state_nr_) const -> size_t {
 assert(state_nr_ < _state_final_ids_cache.size());
 return _state_final_ids_cache[state_nr_].size();
}

auto StateMachineTables::get_state_final_id(StateNrType state_nr_, size_t index_) const -> IdType {
 assert(state_nr_ < _state_final_ids_cache.size());
 assert(index_ < _state_final_ids_cache[state_nr_].size());
 return _state_final_ids_cache[state_nr_][index_];
}

auto StateMachineTables::get_state_machine() const -> pmt::sm::StateMachine const& {
 return _state_machine;
}

void StateMachineTables::init_cache() {
 _state_final_ids_cache.clear();
 _state_final_ids_cache.resize(_state_machine.get_state_count());

 for (StateNrType state_nr = 0; state_nr < _state_machine.get_state_count(); ++state_nr) {
  pmt::sm::State const* state = _state_machine.get_state(state_nr);
  if (state) {
   state->get_final_ids().for_each_interval([&](Interval<IdType> interval_) {
    for (IdType accept = interval_.get_lower(); accept <= interval_.get_upper(); ++accept) {
     _state_final_ids_cache[state_nr].push_back(accept);
    }
   });
  }
 }
}

}  // namespace pmt::builder