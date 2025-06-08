#include "pmt/parser/builder/parser_reachability_checker.hpp"

#include "pmt/parser/grammar/grammar_data.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"

namespace pmt::parser::builder {
using namespace pmt::base;
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::parser::grammar;

namespace {
class Locals {
 public:
  std::unordered_map<StateNrType, IntervalSet<StateNrType>> _reverse_transitions;
  IntervalSet<StateNrType> _eoi_states;
  AcceptsIndexType _eoi_accept_index;
};

void find_eoi_states(ParserReachabilityChecker::Args const& args_, Locals& locals_) {
 args_._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
   State const& state = *args_._parser_state_machine.get_state(state_nr_);
   if (state.get_accepts().contains(locals_._eoi_accept_index)) {
     locals_._eoi_states.insert(Interval<StateNrType>(state_nr_));
   }
 });

 if (locals_._eoi_states.empty()) {
   throw std::runtime_error("Parser state machine has no EOI states, which is required for reachability checking.");
 }
}

void fill_reverse_transitions(ParserReachabilityChecker::Args const& args_, Locals& locals_) {
 std::unordered_map<AcceptsIndexType, IntervalSet<StateNrType>> close_transitions;

 // First iteration: fill all transitions except for SymbolKindClose
 args_._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType const state_nr_) {
   State const& state = *args_._parser_state_machine.get_state(state_nr_);
   // Epsilon transitions
   state.get_epsilon_transitions().for_each_key([&](StateNrType const state_nr_next_) {
    locals_._reverse_transitions[state_nr_next_].insert(Interval<StateNrType>(state_nr_));
   });
   // Symbol transitions, except for SymbolKindClose which we take store for later
   state.get_symbol_kinds().for_each_key([&](SymbolKindType symbol_kind_) {
    if (symbol_kind_ == SymbolKindClose) {
     state.get_symbol_transitions(symbol_kind_).for_each_key([&](StateNrType const state_nr_next_, SymbolValueType const symbol_) {
      close_transitions[symbol_].insert(Interval<StateNrType>(state_nr_next_));
     });
     return;
    }

    state.get_symbol_transitions(symbol_kind_).for_each_key([&](StateNrType const state_nr_next_, SymbolValueType const) {
     locals_._reverse_transitions[state_nr_next_].insert(Interval<StateNrType>(state_nr_));
    });
   });
 });

  // Second iteration: fill SymbolKindClose transitions
  args_._parser_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
   State const& state = *args_._parser_state_machine.get_state(state_nr_);
   state.get_accepts().for_each_key([&](AcceptsIndexType const accept_index_) {
    if (accept_index_ == locals_._eoi_accept_index) {
     return;
    }
    auto const itr = close_transitions.find(accept_index_);
    if (itr == close_transitions.end()) {
      return;
    }
    itr->second.for_each_key([&](StateNrType const state_nr_next_) {
     locals_._reverse_transitions[state_nr_next_].insert(Interval<StateNrType>(state_nr_));
    });
   });
  });
}

auto follow_reverse_transitions(Locals const& locals_) -> IntervalSet<StateNrType> {
 IntervalSet<StateNrType> visited;
 std::vector<StateNrType> pending;

 auto const push_and_visit = [&](StateNrType state_nr_) {
   if (visited.contains(state_nr_)) {
     return;
   }
   visited.insert(Interval<StateNrType>(state_nr_));
   pending.push_back(state_nr_);
 };

 auto take = [&]() {
  StateNrType const ret = pending.back();
  pending.pop_back();
  return ret;
 };

 locals_._eoi_states.for_each_key(push_and_visit);

 while (!pending.empty()) {
   StateNrType const state_nr_cur = take();
   auto const itr = locals_._reverse_transitions.find(state_nr_cur);
   if (itr == locals_._reverse_transitions.end()) {
     continue;
   }
   itr->second.for_each_key([&](StateNrType const state_nr_next_) {
     push_and_visit(state_nr_next_);
   });
 }

 return visited;
}

void report_reachability_error(IntervalSet<StateNrType> const& unreachable_states_) {
 if (unreachable_states_.empty()) {
  return;
 }

 static size_t const MAX_REPORTED_STATES = 15;
 std::string error_msg = "Parser cannot reach EOI from these states: ";
 std::string delim;
 size_t count = 0;
 unreachable_states_.for_each_key([&](StateNrType const state_nr_) {
  if (count > MAX_REPORTED_STATES) {
   return;
  } else if (count == MAX_REPORTED_STATES) {
   error_msg += ", ...";
   ++count;
   return;
  }
  error_msg += std::exchange(delim, ", ") + std::to_string(state_nr_);
  ++count;
 });
 throw std::runtime_error(error_msg);
}
}

void ParserReachabilityChecker::check_reachability(Args args_) {
 Locals locals;
 locals._eoi_accept_index = args_._fn_lookup_accept_index_by_label(GrammarData::LABEL_EOI);
 find_eoi_states(args_, locals);
 fill_reverse_transitions(args_, locals);
 IntervalSet<StateNrType> const reachable_state_nrs = follow_reverse_transitions(locals);
 IntervalSet<StateNrType> all_state_nrs = args_._parser_state_machine.get_state_nrs();
 report_reachability_error(all_state_nrs.clone_asymmetric_difference(reachable_state_nrs));
}

}