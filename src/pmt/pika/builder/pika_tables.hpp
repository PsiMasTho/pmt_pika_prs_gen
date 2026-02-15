#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/pika/builder/state_machine_tables.hpp"
#include "pmt/pika/meta/charset_literal.hpp"
#include "pmt/pika/meta/id_table.hpp"
#include "pmt/pika/meta/rule_parameters.hpp"
#include "pmt/pika/rt/clause_base.hpp"
#include "pmt/pika/rt/pika_tables_base.hpp"

#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::pika::meta, Grammar)

namespace pmt::pika::builder {

class ExtendedClause : public pmt::pika::rt::ClauseBase {
public:
 pmt::pika::rt::ClauseBase::Tag _tag;
 pmt::pika::rt::IdType _id = 0;
 std::vector<pmt::pika::rt::IdType> _seed_parent_ids;
 std::vector<pmt::pika::rt::IdType> _child_ids;
 pmt::pika::rt::IdType _special_id = 0;
 bool _can_match_zero : 1 = false;

 ExtendedClause(Tag tag_, pmt::pika::rt::IdType id_);
 virtual ~ExtendedClause() = default;

 [[nodiscard]] auto get_tag() const -> Tag override;
 [[nodiscard]] auto get_id() const -> pmt::pika::rt::IdType override;

 [[nodiscard]] auto get_child_id_at(size_t idx_) const -> pmt::pika::rt::IdType override;
 [[nodiscard]] auto get_child_id_count() const -> size_t override;

 [[nodiscard]] auto get_literal_id() const -> pmt::pika::rt::IdType override;
 [[nodiscard]] auto get_rule_id() const -> pmt::pika::rt::IdType override;

 [[nodiscard]] auto get_seed_parent_id_at(size_t idx_) const -> pmt::pika::rt::IdType override;
 [[nodiscard]] auto get_seed_parent_count() const -> size_t override;

 [[nodiscard]] auto can_match_zero() const -> bool override;
};

class PikaTables : public pmt::pika::rt::PikaTablesBase {
 // -$ Types / Constants $-
 // -$ Data $-
 pmt::pika::meta::IdTable _id_table;
 std::vector<ExtendedClause> _clauses;
 std::vector<pmt::pika::meta::CharsetLiteral> _literals;
 std::vector<pmt::pika::meta::RuleParameters> _rule_parameters;

 StateMachineTables _terminal_state_machine_tables;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaTables(pmt::pika::meta::Grammar const& grammar_);

 // --$ Inherited: pmt::pika::rt::PikaTablesBase $--
 [[nodiscard]] auto fetch_clause(pmt::pika::rt::IdType clause_id_) const -> pmt::pika::rt::ClauseBase const& override;
 [[nodiscard]] auto get_clause_count() const -> size_t override;

 [[nodiscard]] auto fetch_rule_parameters(pmt::pika::rt::IdType rule_id_) const -> pmt::pika::rt::RuleParametersBase const& override;
 [[nodiscard]] auto get_rule_count() const -> size_t override;

 [[nodiscard]] auto get_terminal_state_machine_tables() const -> pmt::pika::rt::StateMachineTablesBase const& override;

 // --$ Other $--
 auto fetch_literal(pmt::pika::rt::IdType literal_id_) const -> pmt::pika::meta::CharsetLiteral const&;

 auto get_id_table() const -> pmt::pika::meta::IdTable const&;
 auto get_terminal_state_machine_tables_full() const -> StateMachineTables const&;

private:
 void initialize(pmt::pika::meta::Grammar const& grammar_);
 void determine_can_match_zero();
 void determine_seed_parents();
};

}  // namespace pmt::pika::builder
