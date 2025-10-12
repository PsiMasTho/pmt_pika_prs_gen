#pragma once

#include "pmt/parser/builder/state_machine_tables.hpp"
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
 std::vector<ExtendedClause> _nonterminal_clauses;
 std::vector<StateMachineTables> _terminal_state_machine_tables;
 std::vector<StateMachineTables> _terminal_lookahead_state_machine_tables;
 std::vector<pmt::parser::grammar::RuleParameters> _rule_parameters;
 std::vector<std::string> _rule_ids;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaProgram(pmt::parser::grammar::Grammar const& grammar_);

 // --$ Inherited: pmt::parser::rt::PikaProgramBase $--
 [[nodiscard]] auto fetch_nonterminal_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& override;
 [[nodiscard]] auto get_nonterminal_clause_count() const -> size_t override;

 [[nodiscard]] auto fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> pmt::parser::rt::PikaRuleInfo override;
 [[nodiscard]] auto get_rule_info_count() const -> size_t override;

 [[nodiscard]] auto get_terminal_state_machine_tables(size_t idx_) const -> pmt::parser::rt::StateMachineTablesBase const& override;
 [[nodiscard]] auto get_terminal_state_machine_lookahead_tables(size_t idx_) const -> pmt::parser::rt::StateMachineTablesBase const& override;
 [[nodiscard]] auto get_terminal_state_machine_count() const -> size_t override;

 // --$ Other $--
 auto fetch_rule_parameters(ClauseBase::IdType rule_info_id_) const -> pmt::parser::grammar::RuleParameters const&;

private:
 void initialize(pmt::parser::grammar::Grammar const& grammar_);
 void partition_terminals();
 void determine_can_match_zero();
 void determine_seed_parents();
};

}  // namespace pmt::parser::builder