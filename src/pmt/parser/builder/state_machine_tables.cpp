#include "pmt/parser/builder/state_machine_tables.hpp"

#include <cassert>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::sm;

namespace {}

StateMachineTables::StateMachineTables(StateMachine state_machine_)
 : _state_machine(std::move(state_machine_))
 , _accepts_bitsets_cache(_state_machine.get_state_nrs().empty() ? 0 : _state_machine.get_state_nrs().back() + 1) {
 init_cache();
}

auto StateMachineTables::get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType {
 State const* state = _state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(symbol_) : StateNrInvalid;
}

auto StateMachineTables::get_state_accepts(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
 assert(state_nr_ < _accepts_bitsets_cache.size());
 return _accepts_bitsets_cache[state_nr_].get_chunks();
}

auto StateMachineTables::get_state_machine() const -> StateMachine const& {
 return _state_machine;
}

auto StateMachineTables::get_max_accept() const -> pmt::sm::AcceptsIndexType {
 return _max_accept_cached;
}

void StateMachineTables::init_cache() {
 assert(_accepts_bitsets_cache.size() == _state_machine.get_state_nrs().size());
 for (StateNrType state_nr = 0; Bitset & bitset : _accepts_bitsets_cache) {
  State const* state = _state_machine.get_state(state_nr++);
  if (!state || state->get_accepts().empty()) {
   continue;
  }

  AcceptsIndexType const highest_accept = state->get_accepts().highest();
  bitset.resize(highest_accept + 1, false);
  _max_accept_cached = std::max(_max_accept_cached, highest_accept);
  state->get_accepts().for_each_key([&](AcceptsIndexType accept_index_) { bitset.set(accept_index_, true); });
 }
}

}  // namespace pmt::parser::builder