/* Generated on: 2026-01-12 21:30:54 */
// clang-format off
#pragma once

#include <pmt/parser/rt/pika_program_base.hpp>

namespace pmt::parser::grammar {

class PikaProgram : public pmt::parser::rt::PikaProgramBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::parser::rt::PikaProgramBase $--
 auto fetch_clause(pmt::parser::rt::ClauseBase::IdType clause_id_) const -> pmt::parser::rt::ClauseBase const& override;
 auto get_clause_count() const -> size_t override;

 auto fetch_rule_parameters(pmt::parser::rt::ClauseBase::IdType rule_id_) const -> pmt::parser::rt::RuleParametersBase const& override;
 auto get_rule_count() const -> size_t override;

 auto get_terminal_state_machine_tables() const -> pmt::parser::rt::StateMachineTablesBase const& override;
};

} // namespace pmt::parser::grammar
// clang-format on