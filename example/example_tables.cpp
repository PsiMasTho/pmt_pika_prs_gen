/* Generated on: 2026-02-04 23:35:28 */
// clang-format off
#include "example_tables.hpp"

#include <pmt/ast/id.hpp>
#include <pmt/rt/rule_parameters_base.hpp>
#include <pmt/rt/state_machine_tables_base.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <span>

namespace example {
using namespace pmt::rt;

namespace {

using TerminalTransitionsType = uint8_t;
using TerminalTransitionsOffsetsType = uint8_t;
using TerminalFinalIdsType = uint8_t;
using TerminalFinalIdsOffsetsType = uint8_t;
using ClauseChildIdsOffsetsType = uint8_t;
using ClauseChildIdsType = uint8_t;
using ClauseSeedParentIdsOffsetsType = uint8_t;
using ClauseSeedParentIdsType = uint8_t;
using ClauseSpecialIdType = uint8_t;
using RuleParameterDisplayNameIndirectType = uint8_t;
using RuleParameterIdStringIndirectType = uint8_t;
using RuleParameterIdTableType = uint64_t;
using RuleParameterIdIndirectType = uint8_t;
using ClauseClassIdType = uint8_t;
using ClauseCanMatchZeroType = uint64_t;
using RuleParameterClassIdType = uint8_t;
using RuleParameterBooleansType = uint64_t;

enum : size_t {
 StringTableSize = 0xB,
 TerminalStateCount = 0x4,
 TerminalTransitionsSize = 0x12,
 TerminalTransitionsOffsetsSize = TerminalStateCount + 1,
 TerminalFinalIdsSize = 0x3,
 TerminalFinalIdsOffsetsSize = TerminalStateCount + 1,
 ClauseCount = 0x19,
 ClauseChildIdsSize = 0x1E,
 ClauseChildIdsOffsetsSize = ClauseCount + 1,
 ClauseSeedParentIdsSize = 0x1C,
 ClauseSeedParentIdsOffsetsSize = ClauseCount + 1,
 ClauseCanMatchZeroSize = 0x1,
 RuleParameterCount = 0xD,
 RuleParameterMergeOffset = 0,
 RuleParameterUnpackOffset = 1,
 RuleParameterHideOffset = 2,
 RuleParameterBooleansSize = 0x1,
 RuleParameterIdTableSize = 0x5,
};

template <std::integral CHUNK_T_>
auto get_bit(CHUNK_T_ const* chunks_, size_t idx_) -> bool {
 size_t const bits_per_chunk = sizeof(CHUNK_T_) * CHAR_BIT;
 size_t const chunk_idx = idx_ / bits_per_chunk;
 size_t const bit_idx = idx_ % bits_per_chunk;
 return (chunks_[chunk_idx] >> bit_idx) & 1;
}

std::array<char const* const, StringTableSize> const STRING_TABLE = {
 "A", "A_Flat", "B", "B_L_Recursive", "C", "C_R_Recursive", "Holder", "IdDefault", "S", "__plus_0", "__plus_body_0"
};

std::array<ClauseBase::Tag const, ClauseCount> const CLAUSE_TAGS = {
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, 
 ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Epsilon
};

std::array<ClauseChildIdsType const, ClauseChildIdsSize> const CLAUSE_CHILD_IDS = {
 0x01, 0x02, 0x0C, 0x12, 0x03, 0x04, 0x18, 0x05, 0x06, 0x0B, 0x07, 0x0A, 0x08, 0x09, 0x05, 0x08, 0x0D, 0x0E, 0x18, 0x0F, 
 0x10, 0x0D, 0x11, 0x13, 0x14, 0x18, 0x15, 0x17, 0x16, 0x13
};

std::array<ClauseChildIdsOffsetsType const, ClauseChildIdsOffsetsSize> const CLAUSE_CHILD_IDS_OFFSETS = {
 0x00, 0x01, 0x04, 0x05, 0x07, 0x08, 0x0A, 0x0C, 0x0D, 0x0E, 0x0E, 0x0F, 0x10, 0x11, 0x13, 0x15, 0x16, 0x17, 0x17, 0x18, 
 0x1A, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E
};

std::array<ClauseSeedParentIdsType const, ClauseSeedParentIdsSize> const CLAUSE_SEED_PARENT_IDS = {
 0x00, 0x01, 0x02, 0x03, 0x04, 0x0A, 0x05, 0x06, 0x07, 0x0B, 0x08, 0x05, 0x01, 0x0C, 0x0F, 0x0D, 0x0E, 0x0E, 0x10, 0x01, 
 0x12, 0x17, 0x13, 0x14, 0x15, 0x03, 0x0D, 0x13
};

std::array<ClauseSeedParentIdsOffsetsType const, ClauseSeedParentIdsOffsetsSize> const CLAUSE_SEED_PARENT_IDS_OFFSETS = {
 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 
 0x16, 0x17, 0x18, 0x19, 0x19, 0x1C
};

std::array<ClauseSpecialIdType const, ClauseCount> const CLAUSE_SPECIAL_IDS = {
 0xC, 0x0, 0x5, 0x0, 0x4, 0x0, 0x0, 0x1, 0x0, 0x0, 0x2, 0x3, 0x8, 0x0, 0x0, 0x6, 0x7, 0x1, 0xB, 0x0, 0x0, 0x9, 0x2, 0xA, 
 0x0
};

std::array<ClauseCanMatchZeroType const, ClauseCanMatchZeroSize> const CLAUSE_CAN_MATCH_ZERO = {
 0x18CB00F
};

class Clause : public ClauseBase {
 // -$ Types / Constants $-
 // -$ Data $-
 ClauseClassIdType _id = 0;

public:
 // -$ Functions $-
 // --$ Inherited: pmt::rt::ClauseBase $--
 auto get_tag() const -> ClauseBase::Tag override {
  return CLAUSE_TAGS[_id];
 }

 auto get_id() const -> ClauseBase::IdType override {
  return _id;
 }

 auto get_child_id_at(size_t idx_) const -> ClauseBase::IdType override {
  return CLAUSE_CHILD_IDS[CLAUSE_CHILD_IDS_OFFSETS[_id] + idx_];
 }

 auto get_child_id_count() const -> size_t override {
  return CLAUSE_CHILD_IDS_OFFSETS[_id + 1] - CLAUSE_CHILD_IDS_OFFSETS[_id];
 }

 auto get_literal_id() const -> ClauseBase::IdType override {
  assert(has_literal_id());
  return CLAUSE_SPECIAL_IDS[_id];
 }

 auto get_rule_id() const -> ClauseBase::IdType override {
  assert(has_rule_id());
  return CLAUSE_SPECIAL_IDS[_id];
 }

 auto get_seed_parent_id_at(size_t idx_) const -> ClauseBase::IdType override {
  return CLAUSE_SEED_PARENT_IDS[CLAUSE_SEED_PARENT_IDS_OFFSETS[_id] + idx_];
 }

 auto get_seed_parent_count() const -> size_t override {
  return CLAUSE_SEED_PARENT_IDS_OFFSETS[_id + 1] - CLAUSE_SEED_PARENT_IDS_OFFSETS[_id];
 }

 auto can_match_zero() const -> bool override {
  return get_bit(CLAUSE_CAN_MATCH_ZERO.data(), _id);
 }

 // --$ Other $--
 void set_id(ClauseBase::IdType id_) {
  _id = id_;
 }
};

std::array<TerminalTransitionsType const, TerminalTransitionsSize> const TERMINAL_TRANSITIONS = {
 0x41, 0x42, 0x43, 0x61, 0x62, 0x63, 0x41, 0x42, 0x43, 0x61, 0x62, 0x63, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03
};

std::array<TerminalTransitionsOffsetsType const, TerminalTransitionsOffsetsSize> const TERMINAL_TRANSITIONS_OFFSETS = {
 0x0, 0x6, 0x6, 0x6, 0x6
};

std::array<TerminalFinalIdsType const, TerminalFinalIdsSize> const TERMINAL_FINAL_IDS = {
 0x9, 0x11, 0x16
};

std::array<TerminalFinalIdsOffsetsType const, TerminalFinalIdsOffsetsSize> const TERMINAL_FINAL_IDS_OFFSETS = {
 0x0, 0x0, 0x1, 0x2, 0x3
};

class TerminalTables : public StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Inherited: pmt::rt::StateMachineTablesBase $--
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
    return StateNrInvalid;
   }
   return values[idx - 1];
  }

  if (symbol_ < lowers[idx]) {
   if (idx == 0) {
    return StateNrInvalid;
   }
   if (symbol_ <= uppers[idx - 1]) {
    return values[idx - 1];
   }
   return StateNrInvalid;
  }

  return values[idx];
 }

 auto get_state_final_id_count(StateNrType state_nr_) const -> size_t  override {
  size_t const start = TERMINAL_FINAL_IDS_OFFSETS[state_nr_];
  size_t const end = TERMINAL_FINAL_IDS_OFFSETS[state_nr_ + 1];
  return end - start;
 }

 auto get_state_final_id(StateNrType state_nr_, size_t index_) const -> FinalIdType  override {
  size_t const start = TERMINAL_FINAL_IDS_OFFSETS[state_nr_];
  return TERMINAL_FINAL_IDS[start + index_];
 }
};

std::array<RuleParameterDisplayNameIndirectType const, RuleParameterCount> const RULE_PARAMETER_DISPLAY_NAMES_INDIRECT = {
 0x0, 0xA, 0x9, 0xA, 0x9, 0x1, 0x3, 0x2, 0x3, 0x4, 0x5, 0x5, 0x8
};

std::array<RuleParameterIdStringIndirectType const, RuleParameterCount> const RULE_PARAMETER_ID_STRINGS_INDIRECT = {
 0x0, 0x7, 0x7, 0x7, 0x7, 0x6, 0x6, 0x2, 0x6, 0x4, 0x6, 0x6, 0x7
};

std::array<RuleParameterIdTableType const, RuleParameterIdTableSize> const RULE_PARAMETER_ID_TABLE = {
 0x0, 0x1, 0x2, 0x3, 0xFFFFFFFFFFFFFFFE
};

std::array<RuleParameterIdIndirectType const, RuleParameterCount> const RULE_PARAMETER_ID_TABLE_INDIRECT = {
 0x2, 0x4, 0x4, 0x4, 0x4, 0x3, 0x3, 0x0, 0x3, 0x1, 0x3, 0x3, 0x4
};

std::array<RuleParameterBooleansType const, RuleParameterBooleansSize> const RULE_PARAMETER_BOOLEANS = {
 0x203C281
};

class RuleParameters : public RuleParametersBase {
 // -$ Data $-
 RuleParameterClassIdType _rule_id;

public:
 // -$ Functions $-
 // --$ Inherited: pmt::rt::RuleParametersBase $--
 auto get_display_name() const -> std::string_view override {
  return STRING_TABLE[RULE_PARAMETER_DISPLAY_NAMES_INDIRECT[_rule_id]];
 }

 auto get_id_string() const -> std::string_view override {
  return STRING_TABLE[RULE_PARAMETER_ID_STRINGS_INDIRECT[_rule_id]];
 }

 auto get_id_value() const -> pmt::ast::IdType override {
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

auto ExampleTables::fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& {
 static auto const clauses = []() {
  std::array<Clause, ClauseCount> ret{};
  for (size_t i = 0; i < ClauseCount; ++i) {
   ret[i].set_id(i);
  }
  return ret;
 }();

 return clauses[clause_id_];
}

auto ExampleTables::get_clause_count() const -> size_t {
 return ClauseCount;
}

auto ExampleTables::fetch_rule_parameters(ClauseBase::IdType rule_id_) const -> RuleParametersBase const& {
 static auto const rule_parameters = []() {
  std::array<RuleParameters, RuleParameterCount> ret{};
  for (size_t i = 0; i < RuleParameterCount; ++i) {
   ret[i].set_rule_id(i);
  }
  return ret;
 }();

 return rule_parameters[rule_id_];
}

auto ExampleTables::get_rule_count() const -> size_t {
 return RuleParameterCount;
}

auto ExampleTables::get_terminal_state_machine_tables() const -> StateMachineTablesBase const& {
 static TerminalTables const tables;
 return tables;
}

} // namespace example
// clang-format on
