#include "pmt/parser/builder/pika_program.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/bitset.hpp"
#include "pmt/base/overloaded.hpp"
#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/grammar/rule.hpp"
#include "pmt/parser/rt/pika_program_base.hpp"

#include <cassert>
#include <set>

namespace pmt::parser::builder {

using namespace pmt::base;
using namespace pmt::parser::rt;
using namespace pmt::parser::grammar;

namespace {

using LitSeqFetcher = decltype(Overloaded{[](std::vector<RuleExpression::LiteralType> const& lit_seq_table_, size_t index_) { return index_ < lit_seq_table_.size() ? &lit_seq_table_[index_] : nullptr; },
                                          [](std::vector<RuleExpression::LiteralType> const&, RuleExpression::LiteralType const& item_) {
                                           return &item_;
                                          }});

class LitSeqHasher {
 std::vector<RuleExpression::LiteralType> const& _lit_seq_table;

public:
 using is_transparent = void;  // NOLINT

 explicit LitSeqHasher(std::vector<RuleExpression::LiteralType> const& lit_seq_table_)
  : _lit_seq_table(lit_seq_table_) {
 }

 auto operator()(auto const& item_) const -> size_t {
  size_t seed = Hash::Phi64;
  RuleExpression::LiteralType const* literal = LitSeqFetcher{}(_lit_seq_table, item_);

  if (literal != nullptr) {
   for (IntervalSet<SymbolType> const& interval : *literal) {
    Hash::combine(interval, seed);
   }
  }

  return seed;
 }
};

class LitSeqEq {
 std::vector<RuleExpression::LiteralType> const& _lit_seq_table;

public:
 using is_transparent = void;  // NOLINT

 explicit LitSeqEq(std::vector<RuleExpression::LiteralType> const& lit_seq_table_)
  : _lit_seq_table(lit_seq_table_) {
 }

 auto operator()(auto const& lhs_, auto const& rhs_) const -> bool {
  RuleExpression::LiteralType const* lhs = LitSeqFetcher{}(_lit_seq_table, lhs_);
  RuleExpression::LiteralType const* rhs = LitSeqFetcher{}(_lit_seq_table, rhs_);
  if (lhs == nullptr || rhs == nullptr) {
   return false;
  }
  return *lhs == *rhs;
 }
};

class LitSeqTableCached {
 std::unordered_set<size_t, LitSeqHasher, LitSeqEq> _lit_seq_set_cache;
 std::vector<RuleExpression::LiteralType>& _lit_seq_table;

public:
 explicit LitSeqTableCached(std::vector<RuleExpression::LiteralType>& lit_seq_table_)
  : _lit_seq_set_cache(0, LitSeqHasher(lit_seq_table_), LitSeqEq(lit_seq_table_))
  , _lit_seq_table(lit_seq_table_) {
 }

 auto add_or_get_lit_seq_index(RuleExpression::LiteralType const& item_) {
  auto const itr = _lit_seq_set_cache.find(item_);
  if (itr != _lit_seq_set_cache.end()) {
   return *itr;
  }

  size_t const idx = _lit_seq_table.size();
  _lit_seq_table.push_back(item_);
  _lit_seq_set_cache.insert(idx);
  return idx;
 }
};

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
}

auto PikaProgram::fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& {
 assert(clause_id_ < get_clause_count());
 return _clauses[clause_id_];
}

auto PikaProgram::get_clause_count() const -> size_t {
 return _clauses.size();
}

auto PikaProgram::fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> PikaRuleInfo {
 assert(rule_info_id_ < _rule_parameter_table.size());
 std::string const& id_string = _rule_parameter_table[rule_info_id_]._id_string;
 size_t const idx = pmt::base::binary_find_index(_rule_id_table.begin(), _rule_id_table.end(), id_string);
 GenericId::IdType const id = idx < _rule_id_table.size() ? idx : GenericId::string_to_id(id_string);
 return PikaRuleInfo{._rule_id = id, ._merge = _rule_parameter_table[rule_info_id_]._merge};
}

auto PikaProgram::get_rule_info_count() const -> size_t {
 return _rule_parameter_table.size();
}

auto PikaProgram::lit_seq_match_at(ClauseBase::IdType lit_seq_id_, size_t idx_, SymbolType sym_) const -> bool {
 assert(idx_ < get_lit_seq_length(lit_seq_id_));
 return _lit_seq_table[lit_seq_id_][idx_].contains(sym_);
}

auto PikaProgram::get_lit_seq_length(ClauseBase::IdType lit_seq_id_) const -> size_t {
 assert(lit_seq_id_ < _lit_seq_table.size());
 return _lit_seq_table[lit_seq_id_].size();
}

auto PikaProgram::fetch_lit_seq(ClauseBase::IdType lit_seq_id_) const -> RuleExpression::LiteralType const& {
 assert(lit_seq_id_ < _lit_seq_table.size());
 return _lit_seq_table[lit_seq_id_];
}

auto PikaProgram::get_lit_seq_count() const -> size_t {
 return _lit_seq_table.size();
}

auto PikaProgram::fetch_rule_parameters(ClauseBase::IdType rule_info_id_) const -> pmt::parser::grammar::RuleParameters const& {
 assert(rule_info_id_ < _rule_parameter_table.size());
 return _rule_parameter_table[rule_info_id_];
}

void PikaProgram::initialize(Grammar const& grammar_) {
 LitSeqTableCached lit_seq_table_cached(_lit_seq_table);

 ClauseBase::IdType counter = 0;
 std::unordered_map<ClauseBase::IdType, ClauseBase::IdType> clause_unique_id_to_index;
 std::unordered_map<std::string, ClauseBase::IdType> rule_name_to_definition_clause_unique_id;
 std::unordered_map<std::string, ClauseBase::IdType> rule_name_to_rule_parameter_id;
 std::unordered_map<ClauseBase::IdType, std::unordered_set<ClauseBase::IdType>> registered_rule_map;
 std::vector<std::pair<RuleExpression const*, ClauseBase::IdType>> pending;

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
   rule_name_to_rule_parameter_id[rule_name] = _rule_parameter_table.size();
   _rule_parameter_table.push_back(grammar_.get_rule(rule_name)->_parameters);
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
  clause_unique_id_to_index[unique_id_] = _clauses.size();
  _clauses.emplace_back(std::move(clause_));
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
   case ClauseBase::Tag::Literal: {
    clause_cur._child_ids.push_back(lit_seq_table_cached.add_or_get_lit_seq_index(expr_cur->get_literal()));
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
   _clauses[clause_unique_id_to_index.find(clause_unique_id)->second]._registered_rule_ids.push_back(registered_rule_id);
  }
 }

 // Change clause unique_id to their actual positions
 for (ExtendedClause& clause : _clauses) {
  for (ClauseBase::IdType& child_id : clause._child_ids) {
   if (clause.get_tag() == ClauseBase::Tag::Literal) {
    continue;
   }
   child_id = clause_unique_id_to_index.find(child_id)->second;
  }
 }

 // Fill the rule id table
 std::set<std::string> rule_id_set;
 for (RuleParameters const& rule_parameters : _rule_parameter_table) {
  if (GenericId::is_generic_id(rule_parameters._id_string)) {
   continue;
  }
  rule_id_set.insert(rule_parameters._id_string);
 }
}

void PikaProgram::determine_can_match_zero() {
 pmt::base::Bitset solved(_clauses.size());
 bool changed = true;

 auto const mark = [&](ClauseBase::IdType clause_id_, bool can_match_zero_) {
  if (solved.get(clause_id_)) {
   return;
  }
  solved.set(clause_id_, true);
  _clauses[clause_id_]._can_match_zero = can_match_zero_;
  changed = true;
 };

 while (changed) {
  changed = false;

  for (std::size_t i = _clauses.size(); i-- > 0;) {
   switch (_clauses[i].get_tag()) {
    case ClauseBase::Tag::Sequence:
    case ClauseBase::Tag::Choice: {
     bool const is_sequence = _clauses[i].get_tag() == ClauseBase::Tag::Sequence;
     if (_clauses[i].get_child_id_count() == 0) {
      mark(i, true);
     }

     bool saw_unsolved = false;
     size_t j = 0;
     for (; j < _clauses[i].get_child_id_count(); ++j) {
      ClauseBase::IdType const child_id = _clauses[i].get_child_id_at(j);
      if (!solved.get(child_id)) {
       saw_unsolved = true;
       continue;
      }
      if (_clauses[child_id].can_match_zero() == !is_sequence) {
       mark(i, !is_sequence);
       break;
      }
     }
     if (!solved.get(i) && j == _clauses[i].get_child_id_count() && !saw_unsolved) {
      mark(i, is_sequence);
     }
    } break;
    case ClauseBase::Tag::Identifier: {
     pmt::unreachable();
    } break;
    case ClauseBase::Tag::Literal: {
     mark(i, fetch_lit_seq(_clauses[i].get_child_id_at(0)).empty());
    } break;
    case ClauseBase::Tag::Hidden:
    case ClauseBase::Tag::OneOrMore: {
     if (solved.get(_clauses[i].get_child_id_at(0))) {
      mark(i, _clauses[_clauses[i].get_child_id_at(0)].can_match_zero());
     }
    } break;
    case ClauseBase::Tag::NotFollowedBy:
    case ClauseBase::Tag::Epsilon:
     mark(i, true);
     break;
   }
  }
 }

 assert(solved.all() && "FAILED TO DETERMINE CAN_MATCH_ZERO");
}

}  // namespace pmt::parser::builder