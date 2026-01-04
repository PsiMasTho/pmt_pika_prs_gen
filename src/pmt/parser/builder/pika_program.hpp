#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/builder/state_machine_tables.hpp"
#include "pmt/parser/grammar/charset_literal.hpp"
#include "pmt/parser/grammar/id_table.hpp"
#include "pmt/parser/grammar/rule_parameters.hpp"
#include "pmt/parser/rt/clause_base.hpp"
#include "pmt/parser/rt/pika_program_base.hpp"

#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, Grammar)

namespace pmt::parser::builder {

class ExtendedRuleParameters : public pmt::parser::grammar::RuleParameters {
public:
 ExtendedRuleParameters(pmt::parser::grammar::RuleParameters const& base_, GenericId::IdType id_value_);

 GenericId::IdType _id_value = GenericId::IdUninitialized;
};

class ExtendedClause : public ClauseBase {
public:
 ClauseBase::Tag _tag;
 ClauseBase::IdType _id = 0;
 std::vector<ClauseBase::IdType> _seed_parent_ids;
 std::vector<ClauseBase::IdType> _child_ids;
 ClauseBase::IdType _literal_id = 0;  // -$ Todo $- only _literal_id or _rule_id is used depending on the tag
 ClauseBase::IdType _rule_id = 0;
 bool _can_match_zero : 1 = false;

 ExtendedClause(Tag tag_, ClauseBase::IdType id_);

 [[nodiscard]] auto get_tag() const -> Tag override;
 [[nodiscard]] auto get_id() const -> IdType override;

 [[nodiscard]] auto get_child_id_at(size_t idx_) const -> ClauseBase::IdType override;
 [[nodiscard]] auto get_child_id_count() const -> size_t override;

 [[nodiscard]] auto get_literal_id() const -> IdType override;
 [[nodiscard]] auto get_rule_id() const -> IdType override;

 [[nodiscard]] auto get_seed_parent_id_at(size_t idx_) const -> ClauseBase::IdType override;
 [[nodiscard]] auto get_seed_parent_count() const -> size_t override;

 [[nodiscard]] auto can_match_zero() const -> bool override;
};

class PikaProgram : public pmt::parser::rt::PikaProgramBase {
 // -$ Types / Constants $-
 // -$ Data $-
 pmt::parser::grammar::IdTable _id_table;
 std::vector<ExtendedClause> _clauses;
 std::vector<pmt::parser::grammar::CharsetLiteral> _literals;
 std::vector<ExtendedRuleParameters> _rule_parameters;

 StateMachineTables _literal_state_machine_tables;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaProgram(pmt::parser::grammar::Grammar const& grammar_);

 // --$ Inherited: pmt::parser::rt::PikaProgramBase $--
 [[nodiscard]] auto fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& override;
 [[nodiscard]] auto get_clause_count() const -> size_t override;

 [[nodiscard]] auto fetch_rule_parameters(ClauseBase::IdType rule_id_) const -> pmt::parser::rt::RuleParametersView override;
 [[nodiscard]] auto get_rule_count() const -> size_t override;

 [[nodiscard]] auto get_terminal_state_machine_tables() const -> pmt::parser::rt::StateMachineTablesBase const& override;

 // --$ Other $--
 auto fetch_literal(ClauseBase::IdType literal_id_) const -> pmt::parser::grammar::CharsetLiteral const&;
 auto fetch_extended_rule_parameters(ClauseBase::IdType rule_id_) const -> ExtendedRuleParameters const&;

 auto get_id_table() const -> pmt::parser::grammar::IdTable const&;

private:
 void initialize(pmt::parser::grammar::Grammar const& grammar_);
 void determine_can_match_zero();
 void determine_seed_parents();
};

}  // namespace pmt::parser::builder