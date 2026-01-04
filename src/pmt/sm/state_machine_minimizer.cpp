#include "pmt/sm/state_machine_minimizer.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/sm/state_machine.hpp"

#include <unordered_set>

namespace pmt::sm {
using namespace pmt::base;

namespace {
auto get_alphabet(StateMachine const& state_machine_, std::span<StateNrType const> state_nrs_) -> std::unordered_set<SymbolType> {
 std::unordered_set<SymbolType> ret;

 for (StateNrType const& state_nr_ : state_nrs_) {
  State const& state = *state_machine_.get_state(state_nr_);
  state.get_symbols().for_each_key([&](SymbolType symbol_) { ret.insert(symbol_); });
 }

 return ret;
}

// Map state numbers to contiguous numbers starting from 0 so that we can represent them in a bitset,
// where set operations are faster
class StateNrBitsetMapping {
public:
 std::unordered_map<StateNrType, StateNrType> _forward;
 std::vector<StateNrType> _backward;
};

auto get_mapping(std::span<StateNrType const> state_nrs_) {
 StateNrBitsetMapping mapping;
 mapping._forward.reserve(state_nrs_.size() + 1);  // +1 for the sink state
 mapping._backward.reserve(state_nrs_.size() + 1);

 for (StateNrType const& state_nr_ : state_nrs_) {
  mapping._forward[state_nr_] = mapping._backward.size();
  mapping._backward.push_back(state_nr_);
 }

 // Add the sink state
 mapping._forward[StateNrInvalid] = mapping._backward.size();
 mapping._backward.push_back(StateNrInvalid);

 return mapping;
}

auto get_initial_partitions(StateMachine const& state_machine_, std::span<StateNrType const> state_nrs_, StateNrBitsetMapping const& mapping_) -> std::unordered_set<Bitset> {
 // Initialize partitions where a partition is a set of states that have the same combination of accepts

 // <accepts, state_nrs>
 std::unordered_map<IntervalSet<AcceptsIndexType>, Bitset> by_accepts;

 auto insert = [&](StateNrType state_nr_, IntervalSet<AcceptsIndexType> const& accepts_) {
  auto itr = by_accepts.find(accepts_);
  if (itr == by_accepts.end()) {
   itr = by_accepts.insert_or_assign(accepts_, Bitset(mapping_._backward.size())).first;
  }
  itr->second.set(mapping_._forward.find(state_nr_)->second, true);
 };

 for (StateNrType const& state_nr_ : state_nrs_) {
  State const& state = *state_machine_.get_state(state_nr_);
  IntervalSet<AcceptsIndexType> const& accepts = state.get_accepts();
  insert(state_nr_, accepts);
 }

 // Insert the sink state, it has no accepts
 insert(StateNrInvalid, IntervalSet<AcceptsIndexType>{});

 std::unordered_set<Bitset> ret;
 for (auto& [accepts, state_nrs] : by_accepts) {
  ret.insert(std::move(state_nrs));
 }

 return ret;
}

class Precomp {
public:
 std::unordered_map<StateNrType, std::unordered_map<SymbolType, Bitset>> _rev;
 std::unordered_map<SymbolType, Bitset> _next;
};

auto get_precomp(StateMachine const& state_machine_, std::span<StateNrType const> state_nrs_, StateNrBitsetMapping const& mapping_, std::unordered_set<SymbolType> const& alphabet_) -> Precomp {
 Precomp ret;

 for (SymbolType const& c : alphabet_) {
  Bitset x(mapping_._backward.size());
  for (StateNrType const& state_nr_from_ : state_nrs_) {
   StateNrType const state_nr_to = mapping_._forward.find(state_machine_.get_state(state_nr_from_)->get_symbol_transition(c))->second;
   x.set(state_nr_to, true);
   auto itr = ret._rev[state_nr_to].find(c);
   if (itr == ret._rev[state_nr_to].end()) {
    itr = ret._rev[state_nr_to].insert_or_assign(c, Bitset(mapping_._backward.size())).first;
   }
   itr->second.set(mapping_._forward.find(state_nr_from_)->second, true);
  }
  ret._next[c] = std::move(x);
 }

 return ret;
}

auto lookup_x_in_precomp(Precomp const& precomp_, Bitset const& a_, SymbolType const& c_, StateNrBitsetMapping const& mapping_) -> Bitset {
 Bitset x(mapping_._backward.size());
 precomp_._next.find(c_)->second.clone_and(a_).for_each_bit([&](size_t i_) { x.inplace_or(precomp_._rev.find(i_)->second.find(c_)->second); });
 return x;
}

auto get_equivalence_partitions(StateMachine const& state_machine_, std::span<StateNrType const> state_nrs_, StateNrBitsetMapping const& mapping_, std::unordered_set<SymbolType> const& alphabet_, std::unordered_set<Bitset> initial_partitions_) -> std::unordered_set<Bitset> {
 Precomp const precomp = get_precomp(state_machine_, state_nrs_, mapping_, alphabet_);

 std::unordered_set<Bitset> w = initial_partitions_;

 while (!w.empty()) {
  Bitset const a = *w.begin();
  w.erase(w.begin());

  for (SymbolType const& c : alphabet_) {
   Bitset const x = lookup_x_in_precomp(precomp, a, c, mapping_);

   std::unordered_set<Bitset> p_new;
   for (Bitset const& y : initial_partitions_) {
    Bitset const x_intersect_y = x.clone_and(y);
    Bitset const x_diff_y = y.clone_asymmetric_difference(x);

    size_t const popcnt_x_intersect_y = x_intersect_y.popcnt();
    size_t const popcnt_x_diff_y = x_diff_y.popcnt();

    if (popcnt_x_intersect_y == 0 || popcnt_x_diff_y == 0) {
     p_new.insert(y);
     continue;
    }

    p_new.insert(x_intersect_y);
    p_new.insert(x_diff_y);

    auto const itr = w.find(y);
    if (itr != w.end()) {
     w.erase(itr);
     w.insert(x_intersect_y);
     w.insert(x_diff_y);
    } else {
     (popcnt_x_intersect_y <= popcnt_x_diff_y) ? w.insert(x_intersect_y) : w.insert(x_diff_y);
    }
   }

   initial_partitions_ = std::move(p_new);
  }
 }

 return initial_partitions_;
}

auto get_state_nr_to_equivalence_partitions_mapping(StateMachine const& state_machine_, StateNrBitsetMapping const& mapping_, std::unordered_set<Bitset> const& equivalence_partitions_) -> std::unordered_map<StateNrType, Bitset const*> {
 std::unordered_map<StateNrType, Bitset const*> ret;

 for (Bitset const& equivalence_partition : equivalence_partitions_) {
  equivalence_partition.for_each_bit([&](size_t state_nr_) { ret.insert_or_assign(mapping_._backward[state_nr_], &equivalence_partition); });
 }

 return ret;
}

auto rebuild_minimized_state_machine(StateMachine const& state_machine_, StateNrBitsetMapping const& mapping_, std::unordered_map<StateNrType, Bitset const*> const& equivalence_partition_mapping_) -> StateMachine {
 std::vector<Bitset const*> pending;
 std::unordered_map<Bitset const*, StateNrType> visited;
 StateMachine result;

 auto const take = [&pending]() {
  Bitset const* ret = pending.back();
  pending.pop_back();
  return ret;
 };

 auto const push_and_visit = [&](Bitset const* item_) -> StateNrType {
  auto itr = visited.find(item_);
  if (itr != visited.end()) {
   return itr->second;
  } else {
   itr = visited.insert_or_assign(item_, result.get_unused_state_nr()).first;
  }

  pending.push_back(item_);
  result.get_or_create_state(itr->second);
  return itr->second;
 };

 if (auto const itr = equivalence_partition_mapping_.find(StateNrStart); itr != equivalence_partition_mapping_.end()) {
  push_and_visit(itr->second);
 }

 while (!pending.empty()) {
  Bitset const* equivalence_partition_cur = take();
  StateNrType const state_nr_cur = visited.find(equivalence_partition_cur)->second;

  State const& state_old = *state_machine_.get_state(mapping_._backward[equivalence_partition_cur->countl(false)]);
  result.get_state(state_nr_cur)->add_accepts(state_old.get_accepts());

  // Set up the transitions
  state_old.get_symbol_transitions().for_each_interval([&](StateNrType state_nr_next_old_, Interval<SymbolType> interval_) {
   if (state_nr_next_old_ == StateNrInvalid) {
    return;
   }
   Bitset const& equivalence_partition_next_old = *equivalence_partition_mapping_.find(state_nr_next_old_)->second;
   StateNrType const state_nr_next_new = push_and_visit(&equivalence_partition_next_old);
   result.get_state(state_nr_cur)->add_symbol_transitions(interval_, state_nr_next_new);
  });
 }

 return result;
}

}  // namespace

auto StateMachineMinimizer::minimize(StateMachine const& input_state_machine_) -> StateMachine {
 std::span<StateNrType const> const state_nrs = input_state_machine_.get_state_nrs();
 StateNrBitsetMapping const mapping = get_mapping(state_nrs);
 std::unordered_set<Bitset> const p = get_equivalence_partitions(input_state_machine_, state_nrs, mapping, get_alphabet(input_state_machine_, state_nrs), get_initial_partitions(input_state_machine_, state_nrs, mapping));
 std::unordered_map<StateNrType, Bitset const*> const state_nr_to_equiv_partition = get_state_nr_to_equivalence_partitions_mapping(input_state_machine_, mapping, p);
 return rebuild_minimized_state_machine(input_state_machine_, mapping, state_nr_to_equiv_partition);
}

}  // namespace pmt::sm