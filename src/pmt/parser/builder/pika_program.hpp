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

class ExtendedClause : public pmt::parser::rt::ClauseBase {
public:
 pmt::parser::rt::ClauseBase::Tag _tag;
 pmt::parser::rt::ClauseBase::IdType _id = 0;
 std::vector<pmt::parser::rt::ClauseBase::IdType> _seed_parent_ids;
 std::vector<pmt::parser::rt::ClauseBase::IdType> _child_ids;
 pmt::parser::rt::ClauseBase::IdType _literal_id = 0;  // -$ Todo $- only _literal_id or _rule_id is used depending on the tag
 pmt::parser::rt::ClauseBase::IdType _rule_id = 0;
 bool _can_match_zero : 1 = false;

 ExtendedClause(Tag tag_, pmt::parser::rt::ClauseBase::IdType id_);

 [[nodiscard]] auto get_tag() const -> Tag override;
 [[nodiscard]] auto get_id() const -> IdType override;

 [[nodiscard]] auto get_child_id_at(size_t idx_) const -> pmt::parser::rt::ClauseBase::IdType override;
 [[nodiscard]] auto get_child_id_count() const -> size_t override;

 [[nodiscard]] auto get_literal_id() const -> IdType override;
 [[nodiscard]] auto get_rule_id() const -> IdType override;

 [[nodiscard]] auto get_seed_parent_id_at(size_t idx_) const -> pmt::parser::rt::ClauseBase::IdType override;
 [[nodiscard]] auto get_seed_parent_count() const -> size_t override;

 [[nodiscard]] auto can_match_zero() const -> bool override;
};

class PikaProgram : public pmt::parser::rt::PikaProgramBase {
 // -$ Types / Constants $-
 // -$ Data $-
 pmt::parser::grammar::IdTable _id_table;
 std::vector<ExtendedClause> _clauses;
 std::vector<pmt::parser::grammar::CharsetLiteral> _literals;
 std::vector<pmt::parser::grammar::RuleParameters> _rule_parameters;

 StateMachineTables _literal_state_machine_tables;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaProgram(pmt::parser::grammar::Grammar const& grammar_);

 // --$ Inherited: pmt::parser::rt::PikaProgramBase $--
 [[nodiscard]] auto fetch_clause(pmt::parser::rt::ClauseBase::IdType clause_id_) const -> pmt::parser::rt::ClauseBase const& override;
 [[nodiscard]] auto get_clause_count() const -> size_t override;

 [[nodiscard]] auto fetch_rule_parameters(pmt::parser::rt::ClauseBase::IdType rule_id_) const -> pmt::parser::rt::RuleParametersBase const& override;
 [[nodiscard]] auto get_rule_count() const -> size_t override;

 [[nodiscard]] auto get_terminal_state_machine_tables() const -> pmt::parser::rt::StateMachineTablesBase const& override;

 // --$ Other $--
 auto fetch_literal(pmt::parser::rt::ClauseBase::IdType literal_id_) const -> pmt::parser::grammar::CharsetLiteral const&;

 auto get_id_table() const -> pmt::parser::grammar::IdTable const&;

private:
 void initialize(pmt::parser::grammar::Grammar const& grammar_);
 void determine_can_match_zero();
 void determine_seed_parents();
};

}  // namespace pmt::parser::builder