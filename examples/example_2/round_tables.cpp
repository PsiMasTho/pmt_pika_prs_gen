/* Generated on: 2026-02-15 20:43:02 */
// clang-format off
#include "round_tables.hpp"

#include <pmt/pika/rt/primitives.hpp>
#include <pmt/pika/rt/reserved_ids.hpp>
#include <pmt/pika/rt/clause_base.hpp>
#include <pmt/pika/rt/rule_parameters_base.hpp>
#include <pmt/pika/rt/state_machine_tables_base.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <span>

namespace example {
using namespace pmt::pika::rt;

namespace {

enum IdConstants : IdType {
#include "shared_id_constants-inl.hpp"
};

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
using RuleParameterIdTableType = uint64_t;
using RuleParameterIdIndirectType = uint8_t;
using ClauseClassIdType = uint8_t;
using ClauseCanMatchZeroType = uint64_t;
using RuleParameterClassIdType = uint8_t;
using RuleParameterBooleansType = uint64_t;

enum : size_t {
 StringTableSize = 0xC,
 TerminalStateCount = 0x7,
 TerminalTransitionsSize = 0x15,
 TerminalTransitionsOffsetsSize = TerminalStateCount + 1,
 TerminalFinalIdsSize = 0x6,
 TerminalFinalIdsOffsetsSize = TerminalStateCount + 1,
 ClauseCount = 0x2D,
 ClauseChildIdsSize = 0x3D,
 ClauseChildIdsOffsetsSize = ClauseCount + 1,
 ClauseSeedParentIdsSize = 0x31,
 ClauseSeedParentIdsOffsetsSize = ClauseCount + 1,
 ClauseCanMatchZeroSize = 0x1,
 RuleParameterCount = 0x18,
 RuleParameterMergeOffset = 0,
 RuleParameterUnpackOffset = 1,
 RuleParameterHideOffset = 2,
 RuleParameterBooleansSize = 0x2,
 RuleParameterIdTableSize = 0x3,
};

template <std::integral CHUNK_T_>
auto get_bit(CHUNK_T_ const* chunks_, size_t idx_) -> bool {
 size_t const bits_per_chunk = sizeof(CHUNK_T_) * CHAR_BIT;
 size_t const chunk_idx = idx_ / bits_per_chunk;
 size_t const bit_idx = idx_ % bits_per_chunk;
 return (chunks_[chunk_idx] >> bit_idx) & 1;
}

std::array<char const* const, StringTableSize> const STRING_TABLE = {
 "__hidden_0", "__hidden_1", "__hidden_2", "__plus_0", "__plus_1", "__plus_body_0", "__plus_body_1", "blank", "leaf", 
 "newline", "round", "whitespace"
};

std::array<ClauseBase::Tag const, ClauseCount> const CLAUSE_TAGS = {
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Choice, ClauseBase::Tag::Identifier, ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Sequence, ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Choice, ClauseBase::Tag::Sequence, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, 
 ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::Epsilon
};

std::array<ClauseChildIdsType const, ClauseChildIdsSize> const CLAUSE_CHILD_IDS = {
 0x01, 0x02, 0x0F, 0x11, 0x12, 0x19, 0x28, 0x29, 0x2B, 0x03, 0x04, 0x2C, 0x05, 0x06, 0x0E, 0x07, 0x0D, 0x08, 0x09, 0x0B, 
 0x0A, 0x0C, 0x05, 0x08, 0x10, 0x03, 0x13, 0x18, 0x14, 0x15, 0x16, 0x17, 0x15, 0x2C, 0x15, 0x15, 0x01, 0x1A, 0x2C, 0x1B, 
 0x1C, 0x27, 0x1D, 0x26, 0x1E, 0x1F, 0x20, 0x22, 0x23, 0x03, 0x21, 0x03, 0x24, 0x25, 0x14, 0x01, 0x1B, 0x1E, 0x03, 0x2A, 
 0x03
};

std::array<ClauseChildIdsOffsetsType const, ClauseChildIdsOffsetsSize> const CLAUSE_CHILD_IDS_OFFSETS = {
 0x00, 0x01, 0x09, 0x0A, 0x0C, 0x0D, 0x0F, 0x11, 0x12, 0x14, 0x15, 0x15, 0x16, 0x16, 0x17, 0x18, 0x19, 0x19, 0x1A, 0x1C, 
 0x1D, 0x1F, 0x1F, 0x22, 0x24, 0x25, 0x27, 0x28, 0x2A, 0x2C, 0x2D, 0x31, 0x32, 0x33, 0x33, 0x34, 0x36, 0x37, 0x38, 0x39, 
 0x3A, 0x3B, 0x3C, 0x3C, 0x3D, 0x3D
};

std::array<ClauseSeedParentIdsType const, ClauseSeedParentIdsSize> const CLAUSE_SEED_PARENT_IDS = {
 0x00, 0x18, 0x25, 0x01, 0x02, 0x11, 0x1F, 0x22, 0x28, 0x2B, 0x03, 0x04, 0x0D, 0x05, 0x06, 0x07, 0x0E, 0x08, 0x09, 0x08, 
 0x0B, 0x05, 0x01, 0x0F, 0x12, 0x13, 0x24, 0x14, 0x16, 0x17, 0x16, 0x12, 0x19, 0x1A, 0x26, 0x1B, 0x1C, 0x1D, 0x27, 0x1E, 
 0x1E, 0x20, 0x23, 0x23, 0x1B, 0x29, 0x03, 0x16, 0x19
};

std::array<ClauseSeedParentIdsOffsetsType const, ClauseSeedParentIdsOffsetsSize> const CLAUSE_SEED_PARENT_IDS_OFFSETS = {
 0x00, 0x00, 0x03, 0x04, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 
 0x19, 0x1B, 0x1E, 0x1E, 0x1F, 0x20, 0x20, 0x21, 0x23, 0x24, 0x25, 0x27, 0x28, 0x29, 0x2A, 0x2A, 0x2A, 0x2B, 0x2C, 0x2C, 
 0x2D, 0x2D, 0x2D, 0x2E, 0x2E, 0x31
};

std::array<ClauseSpecialIdType const, ClauseCount> const CLAUSE_SPECIAL_IDS = {
 0x17, 0x00, 0x06, 0x00, 0x05, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x04, 0x07, 0x02, 0x08, 0x00, 0x09, 
 0x00, 0x03, 0x00, 0x00, 0x0A, 0x00, 0x13, 0x00, 0x00, 0x10, 0x00, 0x0B, 0x0C, 0x04, 0x0D, 0x00, 0x0E, 0x0F, 0x11, 0x12, 
 0x14, 0x15, 0x05, 0x16, 0x00
};

std::array<ClauseCanMatchZeroType const, ClauseCanMatchZeroSize> const CLAUSE_CAN_MATCH_ZERO = {
 0x19048242000C
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
 0x09, 0x0A, 0x20, 0x28, 0x29, 0x2C, 0x30, 0x09, 0x0A, 0x20, 0x28, 0x29, 0x2C, 0x39, 0x01, 0x02, 0x01, 0x03, 0x04, 0x05, 
 0x06
};

std::array<TerminalTransitionsOffsetsType const, TerminalTransitionsOffsetsSize> const TERMINAL_TRANSITIONS_OFFSETS = {
 0x0, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7
};

std::array<TerminalFinalIdsType const, TerminalFinalIdsSize> const TERMINAL_FINAL_IDS = {
 0xA, 0xC, 0x10, 0x2A, 0x21, 0x15
};

std::array<TerminalFinalIdsOffsetsType const, TerminalFinalIdsOffsetsSize> const TERMINAL_FINAL_IDS_OFFSETS = {
 0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6
};

class TerminalTables : public StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Inherited: pmt::pika::rt::StateMachineTablesBase $--
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

 auto get_state_final_id(StateNrType state_nr_, size_t index_) const -> IdType  override {
  size_t const start = TERMINAL_FINAL_IDS_OFFSETS[state_nr_];
  return TERMINAL_FINAL_IDS[start + index_];
 }
};

std::array<RuleParameterDisplayNameIndirectType const, RuleParameterCount> const RULE_PARAMETER_DISPLAY_NAMES_INDIRECT = {
 0xB, 0x9, 0x5, 0x3, 0x5, 0x3, 0x7, 0x0, 0x7, 0x8, 0xA, 0x7, 0x2, 0x7, 0x8, 0xA, 0x6, 0x4, 0x6, 0x4, 0x7, 0x1, 0x7, 0xA
};

std::array<RuleParameterIdTableType const, RuleParameterIdTableSize> const RULE_PARAMETER_ID_TABLE = {
 IdConstants::IdRound, IdConstants::IdLeaf, ReservedIds::IdDefault
};

std::array<RuleParameterIdIndirectType const, RuleParameterCount> const RULE_PARAMETER_ID_TABLE_INDIRECT = {
 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x0, 0x2, 0x2, 0x2, 0x1, 0x0, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x0
};

std::array<RuleParameterBooleansType const, RuleParameterBooleansSize> const RULE_PARAMETER_BOOLEANS = {
 0x39C00F003C004200, 0x70
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

auto RoundTables::fetch_clause(IdType clause_id_) const -> ClauseBase const& {
 static auto const clauses = []() {
  std::array<Clause, ClauseCount> ret{};
  for (size_t i = 0; i < ClauseCount; ++i) {
   ret[i].set_id(i);
  }
  return ret;
 }();

 return clauses[clause_id_];
}

auto RoundTables::get_clause_count() const -> size_t {
 return ClauseCount;
}

auto RoundTables::fetch_rule_parameters(IdType rule_id_) const -> RuleParametersBase const& {
 static auto const rule_parameters = []() {
  std::array<RuleParameters, RuleParameterCount> ret{};
  for (size_t i = 0; i < RuleParameterCount; ++i) {
   ret[i].set_rule_id(i);
  }
  return ret;
 }();

 return rule_parameters[rule_id_];
}

auto RoundTables::get_rule_count() const -> size_t {
 return RuleParameterCount;
}

auto RoundTables::get_terminal_state_machine_tables() const -> StateMachineTablesBase const& {
 static TerminalTables const tables;
 return tables;
}

} // namespace example
// clang-format on
