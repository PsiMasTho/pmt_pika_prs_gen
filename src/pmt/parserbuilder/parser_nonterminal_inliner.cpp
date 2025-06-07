#include "pmt/parserbuilder/parser_nonterminal_inliner.hpp"

#include "pmt/util/smct/state_machine.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smct;
using namespace pmt::util::smrt;

namespace {
class Locals {
 public:
 std::unordered_map<AcceptsIndexType, IntervalSet<StateNrType>> _nonterminal_open_state_nrs;
 std::unordered_map<AcceptsIndexType, IntervalSet<StateNrType>> _nonterminal_post_close_state_nrs;
 std::unordered_map<AcceptsIndexType, IntervalSet<StateNrType>> _nonterminal_accept_state_nrs;
};

void fill_maps(ParserNonterminalInliner::Args const& args_, Locals& locals_) {
 args_._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType const state_nr_) {
  State const& state = *args_._parser_state_machine.get_state(state_nr_);
  state.get_symbol_transitions(SymbolKindClose).for_each_key([&](StateNrType const state_nr_next_, SymbolValueType const symbol_) {
   locals_._nonterminal_open_state_nrs[symbol_].insert(Interval<StateNrType>(state_nr_));
   locals_._nonterminal_post_close_state_nrs[symbol_].insert(Interval<StateNrType>(state_nr_next_));
  });
  state.get_accepts().for_each_key([&](AcceptsIndexType const accept_index_) {
   locals_._nonterminal_accept_state_nrs[accept_index_].insert(Interval<StateNrType>(state_nr_));
  });
 });
}

auto get_next_states(ParserNonterminalInliner::Args const& args_, Locals const& locals_, IntervalSet<StateNrType> const& state_nrs_) -> IntervalSet<StateNrType> {
 IntervalSet<StateNrType> ret;

 state_nrs_.for_each_key([&](StateNrType const state_nr_) {
  State const& state = *args_._parser_state_machine.get_state(state_nr_);
  state.get_epsilon_transitions().for_each_key([&](StateNrType const next_state_nr_) {
   ret.insert(Interval<StateNrType>(next_state_nr_));
  });
  state.get_symbol_kinds().for_each_key([&](SymbolKindType symbol_kind_) {
   if (symbol_kind_ == SymbolKindClose) {
    return;
   }
   state.get_symbol_transitions(symbol_kind_).for_each_key([&](StateNrType const next_state_nr_, SymbolValueType const) {
    ret.insert(Interval<StateNrType>(next_state_nr_));
   });
   state.get_accepts().for_each_key([&](AcceptsIndexType const accept_index_) {
    auto const itr = locals_._nonterminal_post_close_state_nrs.find(accept_index_);
    if (itr == locals_._nonterminal_post_close_state_nrs.end()) {
      return;
    }
    itr->second.for_each_key([&](StateNrType const next_state_nr_) {
      ret.insert(Interval<StateNrType>(next_state_nr_));
    });
   });
  });
 });

 return ret;
}

auto get_next_states(ParserNonterminalInliner::Args const& args_, Locals const& locals_, StateNrType state_nr_) -> IntervalSet<StateNrType> {
 return get_next_states(args_, locals_, IntervalSet<StateNrType>(Interval<StateNrType>(state_nr_)));
}

auto check_inlineable(ParserNonterminalInliner::Args const& args_, Locals const& locals_, AcceptsIndexType accepts_index_) -> bool {
 if (locals_._nonterminal_post_close_state_nrs.at(accepts_index_).popcnt() != 1) {
  return false; // Nonterminal must have exactly one post-open state which we can redirect to.
 }
 
 IntervalSet<StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const push_and_visit = [&](StateNrType state_nr_) {
   if (visited.contains(state_nr_)) {
     return;
   }
   visited.insert(Interval<StateNrType>(state_nr_));
   pending.push_back(state_nr_);
 };

 auto const take = [&]() {
  StateNrType const ret = pending.back();
  pending.pop_back();
  return ret;
 };

 IntervalSet<StateNrType> const state_nrs_open = locals_._nonterminal_open_state_nrs.at(accepts_index_);
 get_next_states(args_, locals_, state_nrs_open).for_each_key(push_and_visit);

 while (!pending.empty()) {
  StateNrType const state_nr_cur = take();
  get_next_states(args_, locals_, state_nr_cur).for_each_key(push_and_visit);
 }

 return visited.clone_and(state_nrs_open).empty();
}

void do_inline(ParserNonterminalInliner::Args& args_, Locals const& locals_, AcceptsIndexType accepts_index_) {
 locals_._nonterminal_open_state_nrs.at(accepts_index_).for_each_key([&](StateNrType const state_nr_open_) {
  State& state_open = *args_._parser_state_machine.get_state(state_nr_open_);
  state_open.remove_symbol_transition(Symbol(SymbolKindClose, accepts_index_));
  StateNrType const state_nr_post_open = state_open.get_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen));
  state_open.remove_symbol_transition(Symbol(SymbolKindOpen, SymbolValueOpen));
  state_open.add_epsilon_transition(state_nr_post_open);
 });

 locals_._nonterminal_accept_state_nrs.at(accepts_index_).for_each_key([&](StateNrType const state_nr_accept_) {
  State& state_accept = *args_._parser_state_machine.get_state(state_nr_accept_);
  state_accept.get_accepts().erase(Interval<AcceptsIndexType>(accepts_index_));
  locals_._nonterminal_post_close_state_nrs.at(accepts_index_).for_each_key([&](StateNrType const state_nr_post_close_) {
   state_accept.add_epsilon_transition(state_nr_post_close_);
  });
 });
}

}

void ParserNonterminalInliner::unpack(Args args_) {
 Locals locals;

 fill_maps(args_, locals);

 args_._nonterminals_to_inline.for_each_key([&](AcceptsIndexType const accepts_index_) {
  if (!check_inlineable(args_, locals, accepts_index_)) {
   return;
  }
  do_inline(args_, locals, accepts_index_);
 });
}

}