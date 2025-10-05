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
 std::vector<ClauseBase::IdType> _registered_rule_ids;
 bool _can_match_zero : 1 = false;

 explicit ExtendedClause(Tag tag_);

 [[nodiscard]] auto get_tag() const -> Tag override;

 [[nodiscard]] auto get_child_id_at(size_t idx_) const -> ClauseBase::IdType override;
 [[nodiscard]] auto get_child_id_count() const -> size_t override;

 [[nodiscard]] auto get_registered_rule_id_at(size_t idx_) const -> ClauseBase::IdType override;
 [[nodiscard]] auto get_registered_rule_id_count() const -> size_t override;

 [[nodiscard]] auto get_seed_parent_id_at(size_t idx_) const -> ClauseBase::IdType override;
 [[nodiscard]] auto get_seed_parent_count() const -> size_t override;

 [[nodiscard]] auto can_match_zero() const -> bool override;
};

class PikaProgram : public pmt::parser::rt::PikaProgramBase {
 // -$ Types / Constants $-
 // -$ Data $-
 std::vector<ExtendedClause> _clauses;
 std::vector<pmt::parser::grammar::RuleParameters> _rule_parameter_table;
 std::vector<pmt::parser::grammar::RuleExpression::LiteralType> _lit_seq_table;
 std::vector<std::string> _rule_id_table;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaProgram(pmt::parser::grammar::Grammar const& grammar_);

 // --$ Inherited: pmt::parser::rt::PikaProgramBase $--
 [[nodiscard]] auto fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& override;
 [[nodiscard]] auto get_clause_count() const -> size_t override;

 [[nodiscard]] auto fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> pmt::parser::rt::PikaRuleInfo override;
 [[nodiscard]] auto get_rule_info_count() const -> size_t override;

 [[nodiscard]] auto lit_seq_match_at(ClauseBase::IdType lit_seq_id_, size_t idx_, SymbolType sym_) const -> bool override;
 [[nodiscard]] auto get_lit_seq_length(ClauseBase::IdType lit_seq_id_) const -> size_t override;

 // --$ Other $--
 auto fetch_lit_seq(ClauseBase::IdType lit_seq_id_) const -> pmt::parser::grammar::RuleExpression::LiteralType const&;
 auto get_lit_seq_count() const -> size_t;

 auto fetch_rule_parameters(ClauseBase::IdType rule_info_id_) const -> pmt::parser::grammar::RuleParameters const&;

private:
 void initialize(pmt::parser::grammar::Grammar const& grammar_);
 void partition_terminals();
 void determine_can_match_zero();
 void determine_seed_parents();
};

}  // namespace pmt::parser::builder