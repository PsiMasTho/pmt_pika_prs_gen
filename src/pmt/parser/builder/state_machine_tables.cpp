#include "pmt/parser/builder/state_machine_tables.hpp"

#include "pmt/util/sm/ct/state.hpp"

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;

namespace {}

StateMachineTables::StateMachineTables(StateMachine state_machine_, std::unordered_set<SymbolKindType> const& symbol_kinds_to_cache_transitions_for_)
 : _state_machine(std::move(state_machine_)) {
 fill_accepts_bitsets_cache();

 for (SymbolKindType const kind : symbol_kinds_to_cache_transitions_for_) {
  fill_transitions_bitset_cache(kind);
 }
}

auto StateMachineTables::get_state_nr_next(StateNrType state_nr_, SymbolKindType kind_, SymbolValueType symbol_) const -> StateNrType {
 State const* state = _state_machine.get_state(state_nr_);
 return state ? state->get_symbol_transition(Symbol(kind_, symbol_)) : StateNrSink;
}

auto StateMachineTables::get_state_transitions(StateNrType state_nr_, SymbolKindType kind_) const -> Bitset::ChunkSpanConst {
 auto const itr1 = _transitions_cached.find(kind_);
 if (itr1 == _transitions_cached.end()) {
  return {};
 }

 auto const itr2 = itr1->second.find(state_nr_);
 return itr2 == itr1->second.end() ? Bitset::ChunkSpanConst{} : itr2->second.get_chunks();
}

auto StateMachineTables::get_state_accepts(StateNrType state_nr_) const -> Bitset::ChunkSpanConst {
 auto const itr = _accepts_bitsets_cache.find(state_nr_);
 return (itr != _accepts_bitsets_cache.end()) ? itr->second.get_chunks() : Bitset::ChunkSpanConst{};
}

auto StateMachineTables::get_accept_count() const -> size_t {
 return _accept_count;
}

void StateMachineTables::fill_accepts_bitsets_cache() {
 _accept_count = 0;

 _accepts_bitsets_cache.clear();
 _state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const* state = _state_machine.get_state(state_nr_);
  if (!state) {
   return;
  }

  Bitset& bitset = _accepts_bitsets_cache[state_nr_];
  size_t const needed_size = (state->get_accepts().empty()) ? 0 : state->get_accepts().highest() + 1;
  bitset.resize(needed_size, false);
  state->get_accepts().for_each_key([&](AcceptsIndexType accept_index_) { bitset.set(accept_index_, true); });
  _accept_count = std::max(needed_size, _accept_count);
 });
}

void StateMachineTables::fill_transitions_bitset_cache(pmt::util::sm::SymbolKindType kind_) {
 _state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_state_machine.get_state(state_nr_);
  IntervalMap<SymbolValueType, StateNrType> const transitions = state.get_symbol_transitions(kind_);
  Bitset& mask = _transitions_cached[kind_][state_nr_];
  transitions.for_each_key([&](StateNrType, SymbolValueType symbol_) {
   if (mask.size() <= symbol_) {
    mask.resize(symbol_ + 1, false);
   }

   mask.set(symbol_, true);
  });
 });
}

}  // namespace pmt::parser::builder