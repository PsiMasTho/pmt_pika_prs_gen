#include "pmt/parser/builder/lookahead_terminal_transition_mask_builder.hpp"

#include "pmt/base/interval_set.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;

namespace {

class Locals {
public:
 std::unordered_map<AcceptsIndexType, IntervalSet<StateNrType>> _accepts_to_lookahead_state_nrs;
 std::unordered_map<StateNrType, IntervalSet<AcceptsIndexType>> _parser_state_nrs_to_lookahead_accepts;
 std::unordered_map<StateNrType, std::unordered_map<StateNrType, IntervalSet<SymbolValueType>>> _reverse_lookahead_transitions;
};

void fill_accepts_to_lookahead_state_nrs(LookaheadTerminalTransitionMaskBuilder::Args const& args_, Locals& locals_) {
 args_._lookahead_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *args_._lookahead_state_machine.get_state(state_nr_);
  state.get_accepts().for_each_key([&](AcceptsIndexType accepts_index_) { locals_._accepts_to_lookahead_state_nrs[accepts_index_].insert(Interval(state_nr_)); });
 });
}

void fill_parser_state_nrs_to_lookahead_accepts(LookaheadTerminalTransitionMaskBuilder::Args const& args_, Locals& locals_) {
 args_._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  // The SymbolKindConflict transitions will become the accepts
  State const& state = *args_._parser_state_machine.get_state(state_nr_);
  state.get_symbol_transitions(SymbolKindConflict).for_each_key([&](StateNrType, SymbolValueType sym_) { locals_._parser_state_nrs_to_lookahead_accepts[state_nr_].insert(Interval(sym_)); });
 });
}

void fill_reverse_lookahead_transitions(LookaheadTerminalTransitionMaskBuilder::Args const& args_, Locals& locals_) {
 args_._lookahead_state_machine.get_state_nrs().for_each_key([&](StateNrType lookahead_state_nr_) {
  State const& state = *args_._lookahead_state_machine.get_state(lookahead_state_nr_);
  state.get_symbol_transitions(SymbolKindTerminal).for_each_key([&](StateNrType next_state_nr_, SymbolValueType sym_) { locals_._reverse_lookahead_transitions[next_state_nr_][lookahead_state_nr_].insert(Interval(sym_)); });
 });
}

auto build_lookahead_transition_mask(StateNrType state_nr_parser_, LookaheadTerminalTransitionMaskBuilder::Args const& args_, Locals const& locals_) -> std::unordered_map<StateNrType, Bitset> {
 IntervalSet<StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const take = [&]() {
  auto item = pending.back();
  pending.pop_back();
  return item;
 };

 auto const push_and_visit = [&](StateNrType state_nr_) {
  if (visited.contains(state_nr_)) {
   return;
  }

  visited.insert(Interval(state_nr_));
  pending.push_back(state_nr_);
 };

 // Start with all the states that are accepting for the given parser state
 locals_._parser_state_nrs_to_lookahead_accepts.find(state_nr_parser_)->second.for_each_key([&](AcceptsIndexType accept_index_) { locals_._accepts_to_lookahead_state_nrs.find(accept_index_)->second.for_each_key(push_and_visit); });

 std::unordered_map<StateNrType, Bitset> lookahead_transition_mask;

 while (!pending.empty()) {
  StateNrType const state_nr_cur = take();

  auto const itr_reverse_transitions = locals_._reverse_lookahead_transitions.find(state_nr_cur);
  if (itr_reverse_transitions == locals_._reverse_lookahead_transitions.end()) {
   continue;  // No transitions from this state
  }

  auto const& reverse_transitions = itr_reverse_transitions->second;

  for (auto const& [state_nr_from, symbols] : reverse_transitions) {
   push_and_visit(state_nr_from);
   Bitset& mask = lookahead_transition_mask[state_nr_from];
   symbols.for_each_key([&](SymbolValueType sym_) {
    if (mask.size() <= sym_) {
     mask.resize(sym_ + 1, false);
    }

    mask.set(sym_, true);
   });
  }
 }

 return lookahead_transition_mask;
}

}  // namespace

auto LookaheadTerminalTransitionMaskBuilder::build(Args args_) -> std::unordered_map<StateNrType, std::unordered_map<StateNrType, Bitset>> {
 Locals locals;

 fill_accepts_to_lookahead_state_nrs(args_, locals);
 fill_parser_state_nrs_to_lookahead_accepts(args_, locals);
 fill_reverse_lookahead_transitions(args_, locals);

 std::unordered_map<StateNrType, std::unordered_map<StateNrType, Bitset>> lookahead_transition_masks;
 for (auto const& [state_nr_parser, _] : locals._parser_state_nrs_to_lookahead_accepts) {
  lookahead_transition_masks[state_nr_parser] = build_lookahead_transition_mask(state_nr_parser, args_, locals);
 }

 return lookahead_transition_masks;
}

}  // namespace pmt::parser::builder