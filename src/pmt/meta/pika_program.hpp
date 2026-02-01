/* Generated on: 2026-02-01 19:09:16 */
// clang-format off
#pragma once

#include <pmt/rt/pika_program_base.hpp>

namespace pmt::meta {

class PikaProgram : public pmt::rt::PikaProgramBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::rt::PikaProgramBase $--
 auto fetch_clause(pmt::rt::ClauseBase::IdType clause_id_) const -> pmt::rt::ClauseBase const& override;
 auto get_clause_count() const -> size_t override;

 auto fetch_rule_parameters(pmt::rt::ClauseBase::IdType rule_id_) const -> pmt::rt::RuleParametersBase const& override;
 auto get_rule_count() const -> size_t override;

 auto get_terminal_state_machine_tables() const -> pmt::rt::StateMachineTablesBase const& override;
};

} // namespace pmt::meta
// clang-format on