/* Generated on: 2026-02-03 23:47:25 */
// clang-format off
#pragma once

#include <pmt/rt/pika_tables_base.hpp>

namespace example {

class ExampleTables : public pmt::rt::PikaTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::rt::PikaTablesBase $--
 auto fetch_clause(pmt::rt::ClauseBase::IdType clause_id_) const -> pmt::rt::ClauseBase const& override;
 auto get_clause_count() const -> size_t override;

 auto fetch_rule_parameters(pmt::rt::ClauseBase::IdType rule_id_) const -> pmt::rt::RuleParametersBase const& override;
 auto get_rule_count() const -> size_t override;

 auto get_terminal_state_machine_tables() const -> pmt::rt::StateMachineTablesBase const& override;
};

} // namespace example
// clang-format on