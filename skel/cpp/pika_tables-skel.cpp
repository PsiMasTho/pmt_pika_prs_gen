/* Generated on: /* $replace TIMESTAMP */ */
// clang-format off
/* $replace HEADER_INCLUDE */

#include <pmt/rt/primitives.hpp>
#include <pmt/rt/reserved_ids.hpp>
#include <pmt/rt/clause_base.hpp>
#include <pmt/rt/rule_parameters_base.hpp>
#include <pmt/rt/state_machine_tables_base.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <span>

/* $replace NAMESPACE_OPEN */
using namespace pmt::rt;

namespace {

enum IdConstants : IdType {
/* $replace ID_CONSTANTS_INCLUDE */
};

using TerminalTransitionsType = /* $replace TERMINAL_TRANSITIONS_TYPE */;
using TerminalTransitionsOffsetsType = /* $replace TERMINAL_TRANSITIONS_OFFSETS_TYPE */;
using TerminalFinalIdsType = /* $replace TERMINAL_FINAL_IDS_TYPE */;
using TerminalFinalIdsOffsetsType = /* $replace TERMINAL_FINAL_IDS_OFFSETS_TYPE */;
using ClauseChildIdsOffsetsType = /* $replace CLAUSE_CHILD_IDS_OFFSETS_TYPE */;
using ClauseChildIdsType = /* $replace CLAUSE_CHILD_IDS_TYPE */;
using ClauseSeedParentIdsOffsetsType = /* $replace CLAUSE_SEED_PARENT_IDS_OFFSETS_TYPE */;
using ClauseSeedParentIdsType = /* $replace CLAUSE_SEED_PARENT_IDS_TYPE */;
using ClauseSpecialIdType = /* $replace CLAUSE_SPECIAL_ID_TYPE */;
using RuleParameterDisplayNameIndirectType = /* $replace RULE_PARAMETER_DISPLAY_NAME_INDIRECT_TYPE */;
using RuleParameterIdTableType = /* $replace RULE_PARAMETER_ID_TABLE_TYPE */;
using RuleParameterIdIndirectType = /* $replace RULE_PARAMETER_ID_INDIRECT_TYPE */;
using ClauseClassIdType = /* $replace CLAUSE_CLASS_ID_TYPE */;
using ClauseCanMatchZeroType = /* $replace CLAUSE_CAN_MATCH_ZERO_TYPE */;
using RuleParameterClassIdType = /* $replace RULE_PARAMETER_CLASS_ID_TYPE */;
using RuleParameterBooleansType = /* $replace RULE_PARAMETER_BOOLEANS_TYPE */;

enum : size_t {
 StringTableSize = /* $replace STRING_TABLE_SIZE */,
 TerminalStateCount = /* $replace TERMINAL_STATE_COUNT */,
 TerminalTransitionsSize = /* $replace TERMINAL_TRANSITIONS_SIZE */,
 TerminalTransitionsOffsetsSize = TerminalStateCount + 1,
 TerminalFinalIdsSize = /* $replace TERMINAL_FINAL_IDS_SIZE */,
 TerminalFinalIdsOffsetsSize = TerminalStateCount + 1,
 ClauseCount = /* $replace CLAUSE_COUNT */,
 ClauseChildIdsSize = /* $replace CLAUSE_CHILD_IDS_SIZE */,
 ClauseChildIdsOffsetsSize = ClauseCount + 1,
 ClauseSeedParentIdsSize = /* $replace CLAUSE_SEED_PARENT_IDS_SIZE */,
 ClauseSeedParentIdsOffsetsSize = ClauseCount + 1,
 ClauseCanMatchZeroSize = /* $replace CLAUSE_CAN_MATCH_ZERO_SIZE */,
 RuleParameterCount = /* $replace RULE_PARAMETER_COUNT */,
 RuleParameterMergeOffset = 0,
 RuleParameterUnpackOffset = 1,
 RuleParameterHideOffset = 2,
 RuleParameterBooleansSize = /* $replace RULE_PARAMETER_BOOLEANS_SIZE */,
 RuleParameterIdTableSize = /* $replace RULE_PARAMETER_ID_TABLE_SIZE */,
};

template <std::integral CHUNK_T_>
auto get_bit(CHUNK_T_ const* chunks_, size_t idx_) -> bool {
 size_t const bits_per_chunk = sizeof(CHUNK_T_) * CHAR_BIT;
 size_t const chunk_idx = idx_ / bits_per_chunk;
 size_t const bit_idx = idx_ % bits_per_chunk;
 return (chunks_[chunk_idx] >> bit_idx) & 1;
}

std::array<char const* const, StringTableSize> const STRING_TABLE = {
/* $replace STRING_TABLE */
};

std::array<ClauseBase::Tag const, ClauseCount> const CLAUSE_TAGS = {
/* $replace CLAUSE_TAGS */
};

std::array<ClauseChildIdsType const, ClauseChildIdsSize> const CLAUSE_CHILD_IDS = {
/* $replace CLAUSE_CHILD_IDS */
};

std::array<ClauseChildIdsOffsetsType const, ClauseChildIdsOffsetsSize> const CLAUSE_CHILD_IDS_OFFSETS = {
/* $replace CLAUSE_CHILD_IDS_OFFSETS */
};

std::array<ClauseSeedParentIdsType const, ClauseSeedParentIdsSize> const CLAUSE_SEED_PARENT_IDS = {
/* $replace CLAUSE_SEED_PARENT_IDS */
};

std::array<ClauseSeedParentIdsOffsetsType const, ClauseSeedParentIdsOffsetsSize> const CLAUSE_SEED_PARENT_IDS_OFFSETS = {
/* $replace CLAUSE_SEED_PARENT_IDS_OFFSETS */
};

std::array<ClauseSpecialIdType const, ClauseCount> const CLAUSE_SPECIAL_IDS = {
/* $replace CLAUSE_SPECIAL_IDS */
};

std::array<ClauseCanMatchZeroType const, ClauseCanMatchZeroSize> const CLAUSE_CAN_MATCH_ZERO = {
/* $replace CLAUSE_CAN_MATCH_ZERO */
};

class Clause : public ClauseBase {
 // -$ Types / Constants $-
 // -$ Data $-
 ClauseClassIdType _id = 0;

public:
 // -$ Functions $-
 // --$ Inherited: ClauseBase $--
 auto get_tag() const -> ClauseBase::Tag override {
  return CLAUSE_TAGS[_id];
 }

 auto get_id() const -> IdType override {
  return _id;
 }

 auto get_child_id_at(size_t idx_) const -> IdType override {
  return CLAUSE_CHILD_IDS[CLAUSE_CHILD_IDS_OFFSETS[_id] + idx_];
 }

 auto get_child_id_count() const -> size_t override {
  return CLAUSE_CHILD_IDS_OFFSETS[_id + 1] - CLAUSE_CHILD_IDS_OFFSETS[_id];
 }

 auto get_literal_id() const -> IdType override {
  assert(has_literal_id());
  return CLAUSE_SPECIAL_IDS[_id];
 }

 auto get_rule_id() const -> IdType override {
  assert(has_rule_id());
  return CLAUSE_SPECIAL_IDS[_id];
 }

 auto get_seed_parent_id_at(size_t idx_) const -> IdType override {
  return CLAUSE_SEED_PARENT_IDS[CLAUSE_SEED_PARENT_IDS_OFFSETS[_id] + idx_];
 }

 auto get_seed_parent_count() const -> size_t override {
  return CLAUSE_SEED_PARENT_IDS_OFFSETS[_id + 1] - CLAUSE_SEED_PARENT_IDS_OFFSETS[_id];
 }

 auto can_match_zero() const -> bool override {
  return get_bit(CLAUSE_CAN_MATCH_ZERO.data(), _id);
 }

 // --$ Other $--
 void set_id(IdType id_) {
  _id = id_;
 }
};

std::array<TerminalTransitionsType const, TerminalTransitionsSize> const TERMINAL_TRANSITIONS = {
/* $replace TERMINAL_TRANSITIONS */
};

std::array<TerminalTransitionsOffsetsType const, TerminalTransitionsOffsetsSize> const TERMINAL_TRANSITIONS_OFFSETS = {
/* $replace TERMINAL_TRANSITIONS_OFFSETS */
};

std::array<TerminalFinalIdsType const, TerminalFinalIdsSize> const TERMINAL_FINAL_IDS = {
/* $replace TERMINAL_FINAL_IDS */
};

std::array<TerminalFinalIdsOffsetsType const, TerminalFinalIdsOffsetsSize> const TERMINAL_FINAL_IDS_OFFSETS = {
/* $replace TERMINAL_FINAL_IDS_OFFSETS */
};

class TerminalTables : public StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Inherited: pmt::rt::StateMachineTablesBase $--
 auto get_state_nr_start() const -> StateNrType override {
  return /* $replace STATE_NR_START */;
 }

 auto get_state_nr_invalid() const -> StateNrType override {
  return /* $replace STATE_NR_INVALID */;
 }

 auto get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType override {
  size_t const start = TERMINAL_TRANSITIONS_OFFSETS[state_nr_];
  size_t const end = TERMINAL_TRANSITIONS_OFFSETS[state_nr_ + 1];
  size_t const size = end - start;

  std::span<TerminalTransitionsType const> const lowers(&TERMINAL_TRANSITIONS[start], size);

  size_t const uppers_offset = std::size(TERMINAL_TRANSITIONS) / 3;
  std::span<TerminalTransitionsType const> const uppers(&TERMINAL_TRANSITIONS[uppers_offset + start], size);

  size_t const values_offset = 2 * uppers_offset;
  std::span<TerminalTransitionsType const> const values(&TERMINAL_TRANSITIONS[values_offset + start], size);

  size_t const idx = std::distance(lowers.begin(), std::lower_bound(lowers.begin(), lowers.end(), symbol_));

  if (idx == lowers.size()) {
   if (idx == 0 || symbol_ > uppers[idx - 1]) {
    return get_state_nr_invalid();
   }
   return values[idx - 1];
  }

  if (symbol_ < lowers[idx]) {
   if (idx == 0) {
    return get_state_nr_invalid();
   }
   if (symbol_ <= uppers[idx - 1]) {
    return values[idx - 1];
   }
   return get_state_nr_invalid();
  }

  return values[idx];
 }

 auto get_state_final_id_count(StateNrType state_nr_) const -> size_t  override {
  size_t const start = TERMINAL_FINAL_IDS_OFFSETS[state_nr_];
  size_t const end = TERMINAL_FINAL_IDS_OFFSETS[state_nr_ + 1];
  return end - start;
 }

 auto get_state_final_id(StateNrType state_nr_, size_t index_) const -> IdType  override {
  size_t const start = TERMINAL_FINAL_IDS_OFFSETS[state_nr_];
  return TERMINAL_FINAL_IDS[start + index_];
 }
};

std::array<RuleParameterDisplayNameIndirectType const, RuleParameterCount> const RULE_PARAMETER_DISPLAY_NAMES_INDIRECT = {
/* $replace RULE_PARAMETER_DISPLAY_NAMES_INDIRECT */
};

std::array<RuleParameterIdTableType const, RuleParameterIdTableSize> const RULE_PARAMETER_ID_TABLE = {
/* $replace RULE_PARAMETER_ID_TABLE */
};

std::array<RuleParameterIdIndirectType const, RuleParameterCount> const RULE_PARAMETER_ID_TABLE_INDIRECT = {
/* $replace RULE_PARAMETER_ID_INDIRECT */
};

std::array<RuleParameterBooleansType const, RuleParameterBooleansSize> const RULE_PARAMETER_BOOLEANS = {
/* $replace RULE_PARAMETER_BOOLEANS */
};

class RuleParameters : public RuleParametersBase {
 // -$ Data $-
 RuleParameterClassIdType _rule_id;

public:
 // -$ Functions $-
 // --$ Inherited: RuleParametersBase $--
 auto get_display_name() const -> std::string_view override {
  return STRING_TABLE[RULE_PARAMETER_DISPLAY_NAMES_INDIRECT[_rule_id]];
 }

 auto get_id_value() const -> IdType override {
  return RULE_PARAMETER_ID_TABLE[RULE_PARAMETER_ID_TABLE_INDIRECT[_rule_id]];
 } 

 auto get_merge() const -> bool override {
  return get_bit(RULE_PARAMETER_BOOLEANS.data(), _rule_id + (RuleParameterMergeOffset * RuleParameterCount));
 }

 auto get_unpack() const -> bool override {
  return get_bit(RULE_PARAMETER_BOOLEANS.data(), _rule_id + (RuleParameterUnpackOffset * RuleParameterCount));
 }

 auto get_hide() const -> bool override {
  return get_bit(RULE_PARAMETER_BOOLEANS.data(), _rule_id + (RuleParameterHideOffset * RuleParameterCount));
 }

 // --$ Other $--
 void set_rule_id(RuleParameterClassIdType rule_id_) {
  _rule_id = rule_id_;
 }
};

} // namespace

auto /* $replace CLASS_NAME */::fetch_clause(IdType clause_id_) const -> ClauseBase const& {
 static auto const clauses = []() {
  std::array<Clause, ClauseCount> ret{};
  for (size_t i = 0; i < ClauseCount; ++i) {
   ret[i].set_id(i);
  }
  return ret;
 }();

 return clauses[clause_id_];
}

auto /* $replace CLASS_NAME */::get_clause_count() const -> size_t {
 return ClauseCount;
}

auto /* $replace CLASS_NAME */::fetch_rule_parameters(IdType rule_id_) const -> RuleParametersBase const& {
 static auto const rule_parameters = []() {
  std::array<RuleParameters, RuleParameterCount> ret{};
  for (size_t i = 0; i < RuleParameterCount; ++i) {
   ret[i].set_rule_id(i);
  }
  return ret;
 }();

 return rule_parameters[rule_id_];
}

auto /* $replace CLASS_NAME */::get_rule_count() const -> size_t {
 return RuleParameterCount;
}

auto /* $replace CLASS_NAME */::get_terminal_state_machine_tables() const -> StateMachineTablesBase const& {
 static TerminalTables const tables;
 return tables;
}

/* $replace NAMESPACE_CLOSE */
// clang-format on
