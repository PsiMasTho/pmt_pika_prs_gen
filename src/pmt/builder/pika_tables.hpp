#pragma once

#include "pmt/builder/state_machine_tables.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/meta/charset_literal.hpp"
#include "pmt/meta/id_table.hpp"
#include "pmt/meta/rule_parameters.hpp"
#include "pmt/rt/clause_base.hpp"
#include "pmt/rt/pika_tables_base.hpp"

#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::meta, Grammar)

namespace pmt::builder {

class ExtendedClause : public pmt::rt::ClauseBase {
public:
 pmt::rt::ClauseBase::Tag _tag;
 pmt::rt::IdType _id = 0;
 std::vector<pmt::rt::IdType> _seed_parent_ids;
 std::vector<pmt::rt::IdType> _child_ids;
 pmt::rt::IdType _special_id = 0;
 bool _can_match_zero : 1 = false;

 ExtendedClause(Tag tag_, pmt::rt::IdType id_);
 virtual ~ExtendedClause() = default;

 [[nodiscard]] auto get_tag() const -> Tag override;
 [[nodiscard]] auto get_id() const -> pmt::rt::IdType override;

 [[nodiscard]] auto get_child_id_at(size_t idx_) const -> pmt::rt::IdType override;
 [[nodiscard]] auto get_child_id_count() const -> size_t override;

 [[nodiscard]] auto get_literal_id() const -> pmt::rt::IdType override;
 [[nodiscard]] auto get_rule_id() const -> pmt::rt::IdType override;

 [[nodiscard]] auto get_seed_parent_id_at(size_t idx_) const -> pmt::rt::IdType override;
 [[nodiscard]] auto get_seed_parent_count() const -> size_t override;

 [[nodiscard]] auto can_match_zero() const -> bool override;
};

class PikaTables : public pmt::rt::PikaTablesBase {
 // -$ Types / Constants $-
 // -$ Data $-
 pmt::meta::IdTable _id_table;
 std::vector<ExtendedClause> _clauses;
 std::vector<pmt::meta::CharsetLiteral> _literals;
 std::vector<pmt::meta::RuleParameters> _rule_parameters;

 StateMachineTables _terminal_state_machine_tables;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaTables(pmt::meta::Grammar const& grammar_);

 // --$ Inherited: pmt::rt::PikaTablesBase $--
 [[nodiscard]] auto fetch_clause(pmt::rt::IdType clause_id_) const -> pmt::rt::ClauseBase const& override;
 [[nodiscard]] auto get_clause_count() const -> size_t override;

 [[nodiscard]] auto fetch_rule_parameters(pmt::rt::IdType rule_id_) const -> pmt::rt::RuleParametersBase const& override;
 [[nodiscard]] auto get_rule_count() const -> size_t override;

 [[nodiscard]] auto get_terminal_state_machine_tables() const -> pmt::rt::StateMachineTablesBase const& override;

 // --$ Other $--
 auto fetch_literal(pmt::rt::IdType literal_id_) const -> pmt::meta::CharsetLiteral const&;

 auto get_id_table() const -> pmt::meta::IdTable const&;
 auto get_terminal_state_machine_tables_full() const -> StateMachineTables const&;

private:
 void initialize(pmt::meta::Grammar const& grammar_);
 void determine_can_match_zero();
 void determine_seed_parents();
};

}  // namespace pmt::builder
