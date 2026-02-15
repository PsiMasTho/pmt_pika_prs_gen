/* Generated on: 2026-02-15 20:46:08 */
// clang-format off
#pragma once

#include <pmt/pika/rt/pika_tables_base.hpp>

namespace pmt::pika::meta {

class PikaTables : public pmt::pika::rt::PikaTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::pika::rt::PikaTablesBase $--
 auto fetch_clause(pmt::pika::rt::IdType clause_id_) const -> pmt::pika::rt::ClauseBase const& override;
 auto get_clause_count() const -> size_t override;

 auto fetch_rule_parameters(pmt::pika::rt::IdType rule_id_) const -> pmt::pika::rt::RuleParametersBase const& override;
 auto get_rule_count() const -> size_t override;

 auto get_terminal_state_machine_tables() const -> pmt::pika::rt::StateMachineTablesBase const& override;
};

} // namespace pmt::pika::meta
// clang-format on