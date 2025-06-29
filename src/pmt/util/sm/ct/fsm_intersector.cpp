#include "pmt/util/sm/ct/fsm_intersector.hpp"

#include "pmt/base/hash.hpp"
#include "pmt/base/hashable.hpp"
#include "pmt/util/sm/ct/state_machine_determinizer.hpp"
#include "util/sm/primitives.hpp"

#include <cassert>

namespace pmt::util::sm::ct {
using namespace pmt::base;

class Locals {
public:
 StateMachine _state_machine_ret;
};

class Item : public Hashable<Item> {
public:
 StateNrType _lhs;
 StateNrType _rhs;

 auto hash() const -> size_t;
};

auto Item::hash() const -> size_t {
 size_t seed = Hash::Phi64;
 Hash::combine(_lhs, seed);
 Hash::combine(_rhs, seed);
 return seed;
}

auto FsmIntersector::intersect(Args args_) -> StateMachine {
 assert(!args_._symbol_kinds_to_follow.empty());

 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = args_._state_machine_lhs, ._state_nr_from = args_._state_nr_from_lhs});
 StateMachineDeterminizer::determinize(StateMachineDeterminizer::Args{._state_machine = args_._state_machine_rhs, ._state_nr_from = args_._state_nr_from_rhs});

 Locals locals;

 std::unordered_map<Item, StateNrType> visited;
 std::vector<Item> pending;

 auto const take = [&]() {
  Item const ret = pending.back();
  pending.pop_back();
  return ret;
 };

 auto const push_and_visit = [&](StateNrType const state_nr_lhs_, StateNrType const state_nrs_rhs_) -> StateNrType {
  Item item{._lhs = state_nr_lhs_, ._rhs = state_nrs_rhs_};
  auto itr = visited.find(item);
  if (itr != visited.end()) {
   return itr->second;
  }
  itr = visited.emplace(item, locals._state_machine_ret.create_new_state()).first;
  pending.push_back(itr->first);
  return itr->second;
 };

 push_and_visit(args_._state_nr_from_lhs, args_._state_nr_from_rhs);

 while (!pending.empty()) {
  Item const item_cur = take();
  StateNrType const state_nr_new_cur = visited.find(item_cur)->second;
  State& state_new_cur = *locals._state_machine_ret.get_state(state_nr_new_cur);

  State const& state_lhs_cur = *args_._state_machine_lhs.get_state(item_cur._lhs);
  State const& state_rhs_cur = *args_._state_machine_rhs.get_state(item_cur._rhs);

  state_new_cur.get_accepts().inplace_or(state_lhs_cur.get_accepts());
  state_new_cur.get_accepts().inplace_or(state_rhs_cur.get_accepts());

  for (SymbolKindType const symbol_kind : args_._symbol_kinds_to_follow) {
   state_lhs_cur.get_symbol_transitions(symbol_kind).for_each_key([&](StateNrType const state_nr_lhs_next_, SymbolValueType const symbol_value_) {
    StateNrType const state_nr_rhs_next = state_rhs_cur.get_symbol_transition(Symbol(symbol_kind, symbol_value_));
    if (state_nr_rhs_next == StateNrSink) {
     return;
    }
    StateNrType const state_nr_new_next = push_and_visit(state_nr_lhs_next_, state_nr_rhs_next);
    state_new_cur.add_symbol_transition(Symbol(symbol_kind, symbol_value_), state_nr_new_next);
   });
  }
 }

 return locals._state_machine_ret;
}

}  // namespace pmt::util::sm::ct