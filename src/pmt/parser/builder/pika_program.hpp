#pragma once

#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/grammar/grammar.hpp"
#include "pmt/parser/grammar/rule.hpp"
#include "pmt/parser/rt/pika_program_base.hpp"

#include <vector>

namespace pmt::parser::builder {

class ExtendedClause : public ClauseBase {
public:
 ClauseBase::Tag _tag;
 std::vector<ClauseBase::IdType> _seed_parent_ids;
 std::vector<ClauseBase::IdType> _child_ids;
 bool _can_match_zero : 1 = false;

 auto get_tag() const -> Tag override;
 auto get_child_id_count() const -> size_t override;
 auto get_seed_parent_count() const -> size_t override;
 auto can_match_zero() const -> bool override;
};

class PikaProgram : public pmt::parser::rt::PikaProgramBase {
 // -$ Types / Constants $-
 // -$ Data $-
 std::vector<ExtendedClause> _clauses;
 std::vector<pmt::parser::rt::PikaRuleInfo> _rule_infos;
 std::vector<pmt::parser::grammar::RuleExpression::LiteralType> _lit_seq_table;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaProgram(pmt::parser::grammar::Grammar const& grammar_);

 // --$ Inherited: pmt::parser::rt::PikaProgramBase $--
 [[nodiscard]] auto fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& override;

 [[nodiscard]] auto get_clause_count() const -> size_t override;

 [[nodiscard]] auto get_child_id_of_clause_at(ClauseBase::IdType clause_id_, size_t idx_) const -> ClauseBase::IdType override;

 [[nodiscard]] auto get_seed_parent_id_of_clause_at(ClauseBase::IdType clause_id_, size_t idx_) const -> ClauseBase::IdType override;

 [[nodiscard]] auto fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> pmt::parser::rt::PikaRuleInfo const& override;

 [[nodiscard]] auto lit_seq_match_at(ClauseBase::IdType lit_seq_id_, size_t idx_, SymbolType sym_) const -> bool override;

 [[nodiscard]] auto get_lit_seq_length(ClauseBase::IdType lit_seq_id_) const -> size_t override;

private:
 void initialize(pmt::parser::grammar::Grammar const& grammar_);
};

}  // namespace pmt::parser::builder