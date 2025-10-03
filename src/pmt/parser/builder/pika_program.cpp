#include "pmt/parser/builder/pika_program.hpp"

#include "parser/grammar/rule.hpp"
#include "pmt/base/overloaded.hpp"

#include <cassert>

namespace pmt::parser::builder {

using namespace pmt::base;
using namespace pmt::parser::rt;
using namespace pmt::parser::grammar;

namespace {

using LitSeqFetcher = decltype(Overloaded{[](std::vector<pmt::parser::grammar::RuleExpression::LiteralType>& lit_seq_table_, size_t index_) { return lit_seq_table_[index_]; },
                                          [](std::vector<pmt::parser::grammar::RuleExpression::LiteralType>&, pmt::parser::grammar::RuleExpression::LiteralType const& item_) {
                                           return item_;
                                          }});

class LitSeqHasher {
 std::vector<pmt::parser::grammar::RuleExpression::LiteralType> const& _lit_seq_table;

public:
 using is_transparent = void;  // NOLINT

 explicit LitSeqHasher(std::vector<pmt::parser::grammar::RuleExpression::LiteralType> const& lit_seq_table_)
  : _lit_seq_table(lit_seq_table_) {
 }

 auto operator()(auto const& item_) const -> size_t {
  return std::hash(LitSeqFetcher{}(_lit_seq_table, item_));
 }
};

class LitSeqEq {
 std::vector<pmt::parser::grammar::RuleExpression::LiteralType> const& _lit_seq_table;

public:
 using is_transparent = void;  // NOLINT

 explicit LitSeqEq(std::vector<pmt::parser::grammar::RuleExpression::LiteralType> const& lit_seq_table_)
  : _lit_seq_table(lit_seq_table_) {
 }

 auto operator()(auto const& lhs_, auto const& rhs_) const -> bool {
  return LitSeqFetcher{}(_lit_seq_table, lhs_) == LitSeqFetcher{}(_lit_seq_table, rhs_);
 }
};

class LitSeqTableCached {
 std::unordered_set<size_t, LitSeqHasher, LitSeqEq> _lit_seq_set_cache;
 std::vector<pmt::parser::grammar::RuleExpression::LiteralType>& _lit_seq_table;

public:
 explicit LitSeqTableCached(std::vector<pmt::parser::grammar::RuleExpression::LiteralType>& lit_seq_table_)
  : _lit_seq_set_cache(0, LitSeqHasher(lit_seq_table_), LitSeqEq(lit_seq_table_))
  , _lit_seq_table(lit_seq_table_) {
 }

 auto add_or_get_lit_seq_index(pmt::parser::grammar::RuleExpression::LiteralType const& item_) {
  auto const itr = _lit_seq_set_cache.find(item_);
  if (itr != _lit_seq_set_cache.end()) {
   return *itr;
  }

  size_t const idx = _lit_seq_table.size() - 1;
  _lit_seq_table.push_back(item_);
  _lit_seq_set_cache.insert(idx);
  return idx;
 }
};

}  // namespace

PikaProgram::PikaProgram(pmt::parser::grammar::Grammar const& grammar_) {
 initialize(grammar_);
}

auto PikaProgram::fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& {
 assert(clause_id_ < get_clause_count());
 return _clauses[clause_id_];
}

auto PikaProgram::get_clause_count() const -> size_t {
 return _clauses.size();
}

auto PikaProgram::get_child_id_of_clause_at(ClauseBase::IdType clause_id_, size_t idx_) const -> ClauseBase::IdType {
 assert(clause_id_ < get_clause_count());
 assert(idx_ < _clauses[clause_id_].get_child_id_count());
 return _clauses[clause_id_]._child_ids[idx_];
}

auto PikaProgram::get_seed_parent_id_of_clause_at(ClauseBase::IdType clause_id_, size_t idx_) const -> ClauseBase::IdType {
 assert(clause_id_ < get_clause_count());
 assert(idx_ < _clauses[clause_id_].get_seed_parent_count());
 return _clauses[clause_id_]._seed_parent_ids[idx_];
}

auto PikaProgram::fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> pmt::parser::rt::PikaRuleInfo const& {
 assert(rule_info_id_ < _rule_infos.size());
 return _rule_infos[rule_info_id_];
}

auto PikaProgram::lit_seq_match_at(ClauseBase::IdType lit_seq_id_, size_t idx_, SymbolType sym_) const -> bool {
 assert(idx_ < get_lit_seq_length(lit_seq_id_));
 return _lit_seq_table[lit_seq_id_][idx_].contains(sym_);
}

auto PikaProgram::get_lit_seq_length(ClauseBase::IdType lit_seq_id_) const -> size_t {
 assert(lit_seq_id_ < _lit_seq_table.size());
 return _lit_seq_table[lit_seq_id_].size();
}

void PikaProgram::initialize(pmt::parser::grammar::Grammar const& grammar_) {
 LitSeqTableCached lit_seq_table_cached(_lit_seq_table);

 std::vector<RuleExpression const*> pending;
 std::unordered_set<std::string> visited_rules;

 auto const push_and_visit = Overloaded{[&](RuleExpression const* expr_) { pending.push_back(expr_); },
                                        [&](std::string const& rule_name_) {
                                         if (visited_rules.insert(rule_name_).second == false) {
                                          return;
                                         }
                                         pending.push_back(grammar_.get_rule(rule_name_)->_definition.get());
                                        }};

 auto const take = [&] {
  RuleExpression const* ret = pending.back();
  pending.pop_back();
  return ret;
 };

 push_and_visit(grammar_.get_start_rule_name());

 while (!pending.empty()) {
  RuleExpression const* cur = take();
 }
}

}  // namespace pmt::parser::builder