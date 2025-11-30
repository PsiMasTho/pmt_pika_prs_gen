#include "pmt/parser/builder/pika_program.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/bitset.hpp"
#include "pmt/base/match.hpp"
#include "pmt/base/overloaded.hpp"
#include "pmt/parser/builder/state_machine_util.hpp"
#include "pmt/parser/builder/terminal_graph_writer.hpp"
#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/grammar/rule.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/parser/rt/pika_program_base.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <cassert>
#include <set>
#include <unordered_set>

namespace pmt::parser::builder {

using namespace pmt::base;
using namespace pmt::parser::rt;
using namespace pmt::parser::grammar;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;

namespace {

using StateMachineFetch = decltype(Overloaded{[](std::vector<StateMachine> const& state_machines_, size_t index_) { return index_ < state_machines_.size() ? &state_machines_[index_] : nullptr; },
                                              [](std::vector<StateMachine> const&, StateMachine const& item_) {
                                               return &item_;
                                              }});

class StateMachineIndirectHash {
 std::vector<StateMachine> const& _state_machines;

public:
 using is_transparent = void;  // NOLINT

 explicit StateMachineIndirectHash(std::vector<StateMachine> const& state_machines_)
  : _state_machines(state_machines_) {
 }

 auto operator()(auto const& item_) const -> size_t {
  return terminal_state_machine_hash(*StateMachineFetch{}(_state_machines, item_), AcceptsEqualityHandling::Present);
 }
};

class StateMachineIndirectEq {
 std::vector<StateMachine> const& _state_machines;

public:
 using is_transparent = void;  // NOLINT

 explicit StateMachineIndirectEq(std::vector<StateMachine> const& state_machines_)
  : _state_machines(state_machines_) {
 }

 auto operator()(auto const& lhs_, auto const& rhs_) const -> bool {
  StateMachine const* lhs = StateMachineFetch{}(_state_machines, lhs_);
  StateMachine const* rhs = StateMachineFetch{}(_state_machines, rhs_);
  if (lhs == nullptr || rhs == nullptr) {
   return false;
  }
  return terminal_state_machine_eq(*lhs, *rhs, AcceptsEqualityHandling::Present);
 }
};

class StateMachinesCached {
 std::vector<StateMachine>& _state_machines;
 Grammar const& _grammar;
 std::unordered_set<size_t, StateMachineIndirectHash, StateMachineIndirectEq> _state_machine_set_cache;
 RuleNameToSymbolFnType const& _rule_name_to_symbol_fn;

public:
 explicit StateMachinesCached(std::vector<StateMachine>& state_machines_, Grammar const& grammar_, RuleNameToSymbolFnType const& rule_name_to_symbol_fn_)
  : _state_machines(state_machines_)
  , _grammar(grammar_)
  , _state_machine_set_cache(0, StateMachineIndirectHash(_state_machines), StateMachineIndirectEq(_state_machines))
  , _rule_name_to_symbol_fn(rule_name_to_symbol_fn_) {
 }

 auto add_or_get_id(RuleExpression const& regular_rule_expression_) -> ClauseBase::IdType {
  ClauseBase::IdType const idx = _state_machines.size();

  StateMachine state_machine = state_machine_from_regular_rule(_grammar, regular_rule_expression_, idx, _rule_name_to_symbol_fn);
  auto const itr = _state_machine_set_cache.find(state_machine);
  if (itr != _state_machine_set_cache.end()) {
   return *itr;
  }

  _state_machines.push_back(std::move(state_machine));
  _state_machine_set_cache.insert(idx);
  return idx;
 }
};

auto create_lookahead_state_machines(std::vector<StateMachine> const& state_machines_) -> std::vector<StateMachine> {
}

void merge_terminal_state_machines(std::vector<StateMachine>& state_machines_) {
}

}  // namespace

ExtendedClause::ExtendedClause(Tag tag_)
 : _tag(tag_) {
}

auto ExtendedClause::get_tag() const -> Tag {
 return _tag;
}

auto ExtendedClause::get_child_id_at(size_t idx_) const -> ClauseBase::IdType {
 assert(idx_ < _child_ids.size());
 return _child_ids[idx_];
}

auto ExtendedClause::get_child_id_count() const -> size_t {
 return _child_ids.size();
}

auto ExtendedClause::get_registered_rule_id_at(size_t idx_) const -> ClauseBase::IdType {
 assert(idx_ < _registered_rule_ids.size());
 return _registered_rule_ids[idx_];
}

auto ExtendedClause::get_registered_rule_id_count() const -> size_t {
 return _registered_rule_ids.size();
}

auto ExtendedClause::get_seed_parent_id_at(size_t idx_) const -> ClauseBase::IdType {
 assert(idx_ < _seed_parent_ids.size());
 return _seed_parent_ids[idx_];
}

auto ExtendedClause::get_seed_parent_count() const -> size_t {
 return _seed_parent_ids.size();
}

auto ExtendedClause::can_match_zero() const -> bool {
 return _can_match_zero;
}

PikaProgram::PikaProgram(Grammar const& grammar_) {
 initialize(grammar_);
 determine_can_match_zero();
 determine_seed_parents();
}

auto PikaProgram::fetch_nonterminal_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& {
 assert(clause_id_ < get_nonterminal_clause_count());
 return _nonterminal_clauses[clause_id_];
}

auto PikaProgram::get_nonterminal_clause_count() const -> size_t {
 return _nonterminal_clauses.size();
}

auto PikaProgram::fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> PikaRuleInfo {
 assert(rule_info_id_ < _rule_parameters.size());
 std::string const& id_string = _rule_parameters[rule_info_id_]._id_string;
 size_t const idx = binary_find_index(_rule_ids.begin(), _rule_ids.end(), id_string);
 GenericId::IdType const id = idx < _rule_ids.size() ? idx : GenericId::string_to_id(id_string);
 return PikaRuleInfo{._rule_id = id, ._merge = _rule_parameters[rule_info_id_]._merge};
}

auto PikaProgram::get_rule_info_count() const -> size_t {
 return _rule_parameters.size();
}

auto PikaProgram::get_terminal_state_machine_tables(size_t idx_) const -> pmt::parser::rt::StateMachineTablesBase const& {
 assert(idx_ < _terminal_state_machine_tables.size());
 return _terminal_state_machine_tables[idx_];
}

auto PikaProgram::get_terminal_state_machine_lookahead_tables(size_t idx_) const -> pmt::parser::rt::StateMachineTablesBase const& {
 assert(idx_ < _terminal_lookahead_state_machine_tables.size());
 return _terminal_lookahead_state_machine_tables[idx_];
}

auto PikaProgram::get_terminal_state_machine_count() const -> size_t {
 return _terminal_state_machine_tables.size();
}

auto PikaProgram::fetch_rule_parameters(ClauseBase::IdType rule_info_id_) const -> pmt::parser::grammar::RuleParameters const& {
 assert(rule_info_id_ < _rule_parameters.size());
 return _rule_parameters[rule_info_id_];
}

void PikaProgram::initialize(Grammar const& grammar_) {
 std::vector<StateMachine> state_machines;

 ClauseBase::IdType counter = 0;
 std::unordered_map<ClauseBase::IdType, ClauseBase::IdType> clause_unique_id_to_index;
 std::unordered_map<std::string, ClauseBase::IdType> rule_name_to_definition_clause_unique_id;
 std::unordered_map<std::string, ClauseBase::IdType> rule_name_to_rule_parameter_id;
 std::unordered_map<ClauseBase::IdType, std::unordered_set<ClauseBase::IdType>> registered_rule_map;
 std::vector<std::pair<RuleExpression const*, ClauseBase::IdType>> pending;

 // Fill rule parameters
 for (std::string const& rule_name : grammar_.get_rule_names()) {
  rule_name_to_rule_parameter_id[rule_name] = _rule_parameters.size();
  _rule_parameters.push_back(grammar_.get_rule(rule_name)->_parameters);
 }

 StateMachinesCached state_machines_cached(state_machines, grammar_, [&](std::string const& rule_name_) -> SymbolValueType {
  auto const itr = rule_name_to_rule_parameter_id.find(rule_name_);
  assert(itr != rule_name_to_rule_parameter_id.end());
  return itr->second;
 });

 auto const push_and_visit = [&](RuleExpression const* expr_) {
  bool is_definition = false;
  RuleExpression const* prev = expr_;
  while (expr_->get_tag() == ClauseBase::Tag::Identifier) {
   std::string const& rule_name = expr_->get_identifier();
   if (auto const itr = rule_name_to_definition_clause_unique_id.find(rule_name); itr != rule_name_to_definition_clause_unique_id.end()) {
    return itr->second;
   }
   prev = expr_;
   expr_ = grammar_.get_rule(expr_->get_identifier())->_definition.get();
  }

  if (prev != expr_) {
   rule_name_to_definition_clause_unique_id[prev->get_identifier()] = counter;
   registered_rule_map[counter].insert(rule_name_to_rule_parameter_id.find(prev->get_identifier())->second);
  }

  pending.emplace_back(expr_, counter);
  return counter++;
 };

 auto const take = [&] {
  auto ret = pending.back();
  pending.pop_back();
  return ret;
 };

 auto const add_clause = [&](ExtendedClause clause_, ClauseBase::IdType unique_id_) {
  clause_unique_id_to_index[unique_id_] = _nonterminal_clauses.size();
  _nonterminal_clauses.emplace_back(std::move(clause_));
 };

 RuleExpression::UniqueHandle const start_expr = RuleExpression::construct(ClauseBase::Tag::Identifier);
 start_expr->set_identifier(grammar_.get_start_rule_name());
 push_and_visit(start_expr.get());

 while (!pending.empty()) {
  auto const [expr_cur, unique_id_cur] = take();

  ExtendedClause clause_cur(expr_cur->get_tag());

  switch (expr_cur->get_tag()) {
   case ClauseBase::Tag::Sequence: {
    for (size_t i = 0; i < expr_cur->get_children_size(); ++i) {
     clause_cur._child_ids.push_back(push_and_visit(expr_cur->get_child_at(i)));
    }
   } break;
   case ClauseBase::Tag::Choice: {
    for (size_t i = 0; i < expr_cur->get_children_size(); ++i) {
     clause_cur._child_ids.push_back(push_and_visit(expr_cur->get_child_at(i)));
    }
   } break;
   case ClauseBase::Tag::Hidden: {
    clause_cur._child_ids.push_back(push_and_visit(expr_cur->get_child_at_front()));
   } break;
   case ClauseBase::Tag::PegRegular:
   case ClauseBase::Tag::CfgRegular: {
    clause_cur._child_ids.push_back(state_machines_cached.add_or_get_id(*expr_cur));
   } break;
   case ClauseBase::Tag::OneOrMore: {
    clause_cur._child_ids.push_back(push_and_visit(expr_cur->get_child_at_front()));
   } break;
   case ClauseBase::Tag::NotFollowedBy: {
    clause_cur._child_ids.push_back(push_and_visit(expr_cur->get_child_at_front()));
   } break;
   case ClauseBase::Tag::Epsilon: {
   } break;
   default:
    pmt::unreachable();
  }
  add_clause(clause_cur, unique_id_cur);
 }

 // Add registered rules
 for (auto const& [clause_unique_id, registered_rule_ids] : registered_rule_map) {
  for (ClauseBase::IdType registered_rule_id : registered_rule_ids) {
   _nonterminal_clauses[clause_unique_id_to_index.find(clause_unique_id)->second]._registered_rule_ids.push_back(registered_rule_id);
  }
 }

 // Change clause unique_id to their actual positions
 for (ExtendedClause& clause : _nonterminal_clauses) {
  for (ClauseBase::IdType& child_id : clause._child_ids) {
   if (Match::is_any_of(clause.get_tag(), ClauseBase::Tag::PegRegular, ClauseBase::Tag::CfgRegular)) {
    continue;
   }
   child_id = clause_unique_id_to_index.find(child_id)->second;
  }
 }

 // Fill the rule id table
 std::set<std::string> rule_id_set;
 for (RuleParameters const& rule_parameters : _rule_parameters) {
  if (GenericId::is_generic_id(rule_parameters._id_string)) {
   continue;
  }
  rule_id_set.insert(rule_parameters._id_string);
 }

 // Write the dotfiles for the state machines individually, maybe remove this eventually
 for (size_t i = 0; StateMachine const& state_machine : state_machines) {
  TerminalGraphWriter(state_machine, std::to_string(i), "terminal_graph_" + std::to_string(i) + ".dot", [&](AcceptsIndexType idx_) { return std::to_string(idx_); }).write_dot();
  ++i;
 }

 merge_terminal_state_machines(state_machines);

 for (StateMachine const& state_machine : state_machines) {
  static std::unordered_set<SymbolKindType> const cached_transition_kinds = {SymbolKindOpen, SymbolKindClose, SymbolKindConflict};
  _terminal_state_machine_tables.emplace_back(state_machine, cached_transition_kinds);
 }
}

void PikaProgram::determine_can_match_zero() {
 Bitset solved(_nonterminal_clauses.size());
 bool changed = true;

 IntervalSet<ClauseBase::IdType> const terminal_can_match_zero_cache = determine_terminal_state_machines_can_match_zero();

 auto const mark = [&](ClauseBase::IdType clause_id_, bool can_match_zero_) {
  if (solved.get(clause_id_)) {
   return;
  }
  solved.set(clause_id_, true);
  _nonterminal_clauses[clause_id_]._can_match_zero = can_match_zero_;
  changed = true;
 };

 while (changed) {
  changed = false;

  for (std::size_t i = _nonterminal_clauses.size(); i-- > 0;) {
   switch (_nonterminal_clauses[i].get_tag()) {
    case ClauseBase::Tag::Sequence:
    case ClauseBase::Tag::Choice: {
     bool const is_sequence = _nonterminal_clauses[i].get_tag() == ClauseBase::Tag::Sequence;
     if (_nonterminal_clauses[i].get_child_id_count() == 0) {
      mark(i, true);
     }

     bool saw_unsolved = false;
     size_t j = 0;
     for (; j < _nonterminal_clauses[i].get_child_id_count(); ++j) {
      ClauseBase::IdType const child_id = _nonterminal_clauses[i].get_child_id_at(j);
      if (!solved.get(child_id)) {
       saw_unsolved = true;
       continue;
      }
      if (_nonterminal_clauses[child_id].can_match_zero() == !is_sequence) {
       mark(i, !is_sequence);
       break;
      }
     }
     if (!solved.get(i) && j == _nonterminal_clauses[i].get_child_id_count() && !saw_unsolved) {
      mark(i, is_sequence);
     }
    } break;
    case ClauseBase::Tag::Identifier: {
     pmt::unreachable();
    } break;
    case ClauseBase::Tag::PegRegular:
    case ClauseBase::Tag::CfgRegular: {
     mark(i, terminal_can_match_zero_cache.contains(_nonterminal_clauses[i].get_child_id_at(0)));
    } break;
    case ClauseBase::Tag::Hidden:
    case ClauseBase::Tag::OneOrMore: {
     if (solved.get(_nonterminal_clauses[i].get_child_id_at(0))) {
      mark(i, _nonterminal_clauses[_nonterminal_clauses[i].get_child_id_at(0)].can_match_zero());
     }
    } break;
    case ClauseBase::Tag::NotFollowedBy:
    case ClauseBase::Tag::Epsilon:
     mark(i, true);
     break;
    default:
     pmt::unreachable();
   }
  }
 }

 assert(solved.all() && "FAILED TO DETERMINE CAN_MATCH_ZERO");
}

auto PikaProgram::determine_terminal_state_machines_can_match_zero() -> IntervalSet<ClauseBase::IdType> {
 IntervalSet<ClauseBase::IdType> ret;
 for (StateMachineTables const& state_machine_tables : _terminal_state_machine_tables) {
  ret.inplace_or(get_accepts_reachable_without_consuming_characters(state_machine_tables.get_state_machine()));
 }

 return ret;
}

void PikaProgram::determine_seed_parents() {
}

void PikaProgram::write_terminal_state_machine_dotfiles() {
 for (size_t i = 0; StateMachineTables const& state_machine_tables : _terminal_state_machine_tables) {
  TerminalGraphWriter(state_machine_tables.get_state_machine(), std::to_string(i), "terminal_graph_combined_" + std::to_string(i) + ".dot", [&](AcceptsIndexType idx_) { return std::to_string(idx_); }).write_dot();
  ++i;
 }
}

}  // namespace pmt::parser::builder