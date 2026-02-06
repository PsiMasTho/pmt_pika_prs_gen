#pragma once

#include "pmt/rt/primitives.hpp"

#include <cstddef>

namespace pmt::rt {
class ClauseBase;
class RuleParametersBase;
class StateMachineTablesBase;

class PikaTablesBase {
public:
 // -$ Functions $-
 // --$ Virtual $--
 [[nodiscard]] virtual auto fetch_clause(IdType clause_id_) const -> ClauseBase const& = 0;
 [[nodiscard]] virtual auto get_clause_count() const -> size_t = 0;

 [[nodiscard]] virtual auto fetch_rule_parameters(IdType rule_id_) const -> RuleParametersBase const& = 0;
 [[nodiscard]] virtual auto get_rule_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_terminal_state_machine_tables() const -> StateMachineTablesBase const& = 0;
};

}  // namespace pmt::rt
