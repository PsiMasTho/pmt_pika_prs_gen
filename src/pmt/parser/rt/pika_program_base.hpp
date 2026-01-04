#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/rt/non_terminal.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::parser::rt, StateMachineTablesBase)

namespace pmt::parser::rt {

class PikaProgramBase {
public:
 // -$ Functions $-
 [[nodiscard]] virtual auto fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& = 0;
 [[nodiscard]] virtual auto get_clause_count() const -> size_t = 0;

 [[nodiscard]] virtual auto fetch_non_terminal(ClauseBase::IdType non_terminal_id_) const -> NonTerminal = 0;
 [[nodiscard]] virtual auto get_non_terminal_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_literal_state_machine_tables() const -> StateMachineTablesBase const& = 0;
};

}  // namespace pmt::parser::rt
