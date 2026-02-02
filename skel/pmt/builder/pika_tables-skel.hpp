/* Generated on: /* $replace TIMESTAMP */ */
// clang-format off
#pragma once

#include <pmt/rt/pika_tables_base.hpp>

/* $replace NAMESPACE_OPEN */

class /* $replace CLASS_NAME */ : public pmt::rt::PikaTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::rt::PikaTablesBase $--
 auto fetch_clause(pmt::rt::ClauseBase::IdType clause_id_) const -> pmt::rt::ClauseBase const& override;
 auto get_clause_count() const -> size_t override;

 auto fetch_rule_parameters(pmt::rt::ClauseBase::IdType rule_id_) const -> pmt::rt::RuleParametersBase const& override;
 auto get_rule_count() const -> size_t override;

 auto get_terminal_state_machine_tables() const -> pmt::rt::StateMachineTablesBase const& override;
};

/* $replace NAMESPACE_CLOSE */
// clang-format on