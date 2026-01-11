#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/rt/clause_base.hpp"
#include "pmt/parser/rt/rule_parameters_base.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::parser::rt, StateMachineTablesBase)

namespace pmt::parser::rt {

class PikaProgramBase {
public:
 // -$ Functions $-
 [[nodiscard]] virtual auto fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& = 0;
 [[nodiscard]] virtual auto get_clause_count() const -> size_t = 0;

 [[nodiscard]] virtual auto fetch_rule_parameters(ClauseBase::IdType rule_id_) const -> RuleParametersBase const& = 0;
 [[nodiscard]] virtual auto get_rule_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_terminal_state_machine_tables() const -> StateMachineTablesBase const& = 0;
};

}  // namespace pmt::parser::rt
