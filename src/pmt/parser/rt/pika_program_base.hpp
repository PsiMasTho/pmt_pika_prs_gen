#pragma once

#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/primitives.hpp"

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
 [[nodiscard]] virtual auto fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& = 0;

 [[nodiscard]] virtual auto get_clause_count() const -> size_t = 0;

 [[nodiscard]] virtual auto get_child_id_of_clause_at(ClauseBase::IdType clause_id_, size_t idx_) const -> ClauseBase::IdType = 0;

 [[nodiscard]] virtual auto get_seed_parent_id_of_clause_at(ClauseBase::IdType clause_id_, size_t idx_) const -> ClauseBase::IdType = 0;

 [[nodiscard]] virtual auto fetch_rule_info(ClauseBase::IdType rule_info_id_) const -> PikaRuleInfo const& = 0;

 [[nodiscard]] virtual auto lit_seq_match_at(ClauseBase::IdType lit_seq_id_, size_t idx_, SymbolType sym_) const -> bool = 0;

 [[nodiscard]] virtual auto get_lit_seq_length(ClauseBase::IdType lit_seq_id_) const -> size_t = 0;
};

}  // namespace pmt::parser::rt
