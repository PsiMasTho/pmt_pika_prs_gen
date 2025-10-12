#pragma once

#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/rt/state_machine_tables_base.hpp"

#include <cstddef>

namespace pmt::parser::rt {

class PikaRuleInfo {
public:
 GenericId::IdType _rule_id;
 bool _merge : 1 = false;

 auto operator==(PikaRuleInfo const& other_) const -> bool = default;
};

class PikaProgramBase {
public:
 // -$ Functions $-
 [[nodiscard]] virtual auto fetch_nonterminal_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& = 0;
 [[nodiscard]] virtual auto get_nonterminal_clause_count() const -> size_t = 0;

 [[nodiscard]] virtual auto fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> PikaRuleInfo = 0;
 [[nodiscard]] virtual auto get_rule_info_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_terminal_state_machine_tables(size_t idx_) const -> StateMachineTablesBase const& = 0;
 [[nodiscard]] virtual auto get_terminal_state_machine_lookahead_tables(size_t idx_) const -> StateMachineTablesBase const& = 0;
 [[nodiscard]] virtual auto get_terminal_state_machine_count() const -> size_t = 0;
};

}  // namespace pmt::parser::rt
