#include "pmt/parser/builder/state_machine_util.hpp"

#include "parser/primitives.hpp"
#include "pmt/base/hash.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "primitives.hpp"

#include <vector>

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;

auto terminal_state_machine_hash(StateMachine const& item_) -> size_t {
 size_t seed = pmt::base::Hash::Phi64;

 std::vector<StateNrType> pending;
 IntervalSet<StateNrType> visited;

 auto const take = [&]() {
  auto const tmp = pending.back();
  pending.pop_back();
  return tmp;
 };

 auto const push_and_visit = [&](StateNrType state_nr_) {
  if (visited.contains(state_nr_)) {
   return;
  }
  visited.insert(Interval(state_nr_));
  pending.push_back(state_nr_);
 };

 if (item_.get_state(StateNrStart) != nullptr) {
  push_and_visit(StateNrStart);
 }

 while (!pending.empty()) {
  StateNrType const state_nr_cur = take();
  State const& state_cur = *item_.get_state(state_nr_cur);

  // Hash combine the accepts
  Hash::combine(state_cur.get_accepts(), seed);

  // Hash combine the SymbolKindCharacter and SymbolKindHiddenCharacter transitions
  Hash::combine(state_cur.get_symbol_values(SymbolKindCharacter), seed);
  Hash::combine(state_cur.get_symbol_values(SymbolKindHiddenCharacter), seed);

  // Follow transitions
  state_cur.get_symbol_transitions(SymbolKindCharacter).for_each_interval([&](StateNrType state_nr_next_, Interval<SymbolValueType> interval_) { push_and_visit(state_nr_next_); });

  state_cur.get_symbol_transitions(SymbolKindHiddenCharacter).for_each_interval([&](StateNrType state_nr_next_, Interval<SymbolValueType> interval_) { push_and_visit(state_nr_next_); });
 }

 return seed;
}

auto terminal_state_machine_eq(StateMachine const& lhs_, StateMachine const& rhs_) -> bool {
 std::vector<std::pair<StateNrType, StateNrType>> pending;
 std::unordered_map<StateNrType, StateNrType> lhs_to_rhs;

 auto const take = [&]() {
  auto const tmp = pending.back();
  pending.pop_back();
  return tmp;
 };

 auto const push_and_visit = [&](StateNrType state_nr_lhs_, StateNrType state_nr_rhs_) {
  lhs_to_rhs[state_nr_lhs_] = state_nr_rhs_;
  lhs_to_rhs[state_nr_rhs_] = state_nr_lhs_;
  pending.emplace_back(state_nr_lhs_, state_nr_rhs_);
 };

 auto const cmp_and_follow_transitions = [&](SymbolKindType kind_, State const& state_lhs_, State const& state_rhs_) {
  auto const& transitions_lhs = state_lhs_.get_symbol_transitions(kind_);
  auto const& transitions_rhs = state_rhs_.get_symbol_transitions(kind_);

  if (transitions_lhs.get_keys() != transitions_rhs.get_keys()) {
   return false;
  }

  for (size_t i = 0; i < transitions_lhs.size(); ++i) {
   auto const [state_nr_lhs_next, _1] = transitions_lhs.get_by_index(i);
   auto const [state_nr_rhs_next, _2] = transitions_rhs.get_by_index(i);

   if (auto const itr = lhs_to_rhs.find(state_nr_lhs_next); itr != lhs_to_rhs.end()) {
    if (state_nr_rhs_next != itr->second) {
     return false;
    }
   } else {
    push_and_visit(state_nr_lhs_next, state_nr_rhs_next);
   }
  }

  return true;
 };

 if (!lhs_.get_state(StateNrStart) || !rhs_.get_state(StateNrStart)) {
  return lhs_.get_state(StateNrStart) == rhs_.get_state(StateNrStart);
 }

 push_and_visit(StateNrStart, StateNrStart);

 while (!pending.empty()) {
  auto const [state_nr_lhs_cur, state_nr_rhs_cur] = take();

  State const& state_cur_lhs = *lhs_.get_state(state_nr_lhs_cur);
  State const& state_cur_rhs = *lhs_.get_state(state_nr_rhs_cur);

  // Compare accepts
  if (state_cur_lhs.get_accepts() != state_cur_rhs.get_accepts()) {
   return false;
  }

  if (!cmp_and_follow_transitions(SymbolKindCharacter, state_cur_lhs, state_cur_rhs) || !cmp_and_follow_transitions(SymbolKindHiddenCharacter, state_cur_lhs, state_cur_rhs)) {
   return false;
  }
 }

 return true;
}

}  // namespace pmt::parser::builder