#include "pmt/parser/builder/state_machine_tables.hpp"

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::sm;

namespace {}

StateMachineTables::StateMachineTables(StateMachine state_machine_)
 : _state_machine(std::move(state_machine_)) {
 fill_accepts_bitsets_cache();
}

auto StateMachineTables::get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType {
 State const* state = _state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(symbol_) : StateNrInvalid;
}

auto StateMachineTables::get_state_accepts(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
 auto const itr = _accepts_bitsets_cache.find(state_nr_);
 return (itr != _accepts_bitsets_cache.end()) ? itr->second.get_chunks() : Bitset::ChunkSpanConst{};
}

auto StateMachineTables::get_accept_count() const -> size_t {
 return _accept_count;
}

auto StateMachineTables::get_state_machine() const -> StateMachine const& {
 return _state_machine;
}

void StateMachineTables::fill_accepts_bitsets_cache() {
 _accept_count = 0;

 _accepts_bitsets_cache.clear();
 for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
  State const* state = _state_machine.get_state(state_nr);
  if (!state) {
   return;
  }

  Bitset& bitset = _accepts_bitsets_cache[state_nr];
  size_t const needed_size = (state->get_accepts().empty()) ? 0 : state->get_accepts().highest() + 1;
  bitset.resize(needed_size, false);
  state->get_accepts().for_each_key([&](AcceptsIndexType accept_index_) { bitset.set(accept_index_, true); });
  _accept_count = std::max(needed_size, _accept_count);
 }
}

}  // namespace pmt::parser::builder