/* Generated on: 2026-02-03 23:47:25 */
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
using TerminalAcceptsType = uint8_t;
using TerminalAcceptsOffsetsType = uint8_t;
using ClauseChildIdsOffsetsType = uint8_t;
using ClauseChildIdsType = uint8_t;
using ClauseSeedParentIdsOffsetsType = uint8_t;
using ClauseSeedParentIdsType = uint8_t;
using ClauseSpecialIdType = uint8_t;
using RuleParameterDisplayNameIndirectType = uint8_t;
using RuleParameterIdStringIndirectType = uint8_t;
using RuleParameterIdTableType = pmt::ast::IdType;
using RuleParameterIdIndirectType = uint8_t;
using ClauseClassIdType = uint8_t;
using RuleParameterClassIdType = uint8_t;
using BitsetChunkType = uint64_t;

template <std::integral CHUNK_T_>
class ChunkBit {
 public:
  inline static constexpr size_t Value = sizeof(CHUNK_T_) * CHAR_BIT;
};

template <std::integral CHUNK_T_, size_t BITCOUNT_>
class ChunksNeeded {
public:
 inline static constexpr size_t Value = (BITCOUNT_ + ChunkBit<CHUNK_T_>::Value - 1) / ChunkBit<CHUNK_T_>::Value;
};

enum : size_t {
 StringTableSize = 0x9,
 TerminalStateCount = 0x6,
 TerminalTransitionsSize = TerminalStateCount * 3, // lowers, uppers, values
 TerminalTransitionsOffsetsSize = TerminalStateCount + 1,
 TerminalAcceptsSize = 0x3,
 TerminalAcceptsOffsetsSize = TerminalStateCount + 1,
 ClauseCount = 0x14,
 ClauseChildIdsSize = 0x17,
 ClauseChildIdsOffsetsSize = ClauseCount + 1,
 ClauseSeedParentIdsSize = 0x16,
 ClauseSeedParentIdsOffsetsSize = ClauseCount + 1,
 ClauseCanMatchZeroSize = ChunksNeeded<BitsetChunkType, ClauseCount>::Value,
 RuleParameterCount = 0x9,
 RuleParameterMergeOffset = 0,
 RuleParameterUnpackOffset = 1,
 RuleParameterHideOffset = 2,
 RuleParameterBooleansSize = ChunksNeeded<BitsetChunkType, RuleParameterCount>::Value * 3, // merge, unpack, hide
 RuleParameterIdTableSize = 0x5
};

template <std::integral CHUNK_T_>
auto get_bit(std::span<CHUNK_T_ const> chunks_, size_t idx_) -> bool {
 size_t const chunk_idx = idx_ / ChunkBit<CHUNK_T_>::Value;
 size_t const bit_idx = idx_ % ChunkBit<CHUNK_T_>::Value;
 return (chunks_[chunk_idx] >> bit_idx) & 1;
}

std::array<char const* const, StringTableSize> const STRING_TABLE = {
 "A", "A_Flat", "B", "B_L_Recursive", "C", "C_R_Recursive", 
 "Holder", "IdDefault", "S"
};

std::array<ClauseBase::Tag const, ClauseCount> const CLAUSE_TAGS = {
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::OneOrMore, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, ClauseBase::Tag::Identifier, 
 ClauseBase::Tag::CharsetLiteral, ClauseBase::Tag::Identifier, ClauseBase::Tag::Choice, ClauseBase::Tag::Sequence, ClauseBase::Tag::Identifier, ClauseBase::Tag::CharsetLiteral, 
 ClauseBase::Tag::Identifier, ClauseBase::Tag::Epsilon
};

std::array<ClauseChildIdsType const, ClauseChildIdsSize> const CLAUSE_CHILD_IDS = {
 0x01, 0x02, 0x07, 0x0D, 0x03, 0x04, 0x13, 0x05, 0x06, 0x08, 0x09, 0x13, 0x0A, 0x0B, 0x08, 0x0C, 0x0E, 0x0F, 0x13, 0x10, 
 0x12, 0x11, 0x0E
};

std::array<ClauseChildIdsOffsetsType const, ClauseChildIdsOffsetsSize> const CLAUSE_CHILD_IDS_OFFSETS = {
 0x00, 0x01, 0x04, 0x05, 0x07, 0x08, 0x09, 0x09, 0x0A, 0x0C, 0x0E, 0x0F, 0x10, 0x10, 0x11, 0x13, 0x15, 0x16, 0x16, 0x17, 
 0x17
};

std::array<ClauseSeedParentIdsType const, ClauseSeedParentIdsSize> const CLAUSE_SEED_PARENT_IDS = {
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x07, 0x0A, 0x08, 0x09, 0x09, 0x0B, 0x01, 0x0D, 0x12, 0x0E, 0x0F, 0x10, 0x03, 
 0x08, 0x0E
};

std::array<ClauseSeedParentIdsOffsetsType const, ClauseSeedParentIdsOffsetsSize> const CLAUSE_SEED_PARENT_IDS_OFFSETS = {
 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x10, 0x11, 0x12, 0x13, 0x13, 
 0x16
};

std::array<ClauseSpecialIdType const, ClauseCount> const CLAUSE_SPECIAL_IDS = {
 0x8, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x2, 0x3, 0x1, 0x7, 0x0, 0x0, 0x5, 0x2, 0x6, 0x0
};

std::array<BitsetChunkType const, ClauseCanMatchZeroSize> const CLAUSE_CAN_MATCH_ZERO = {
 0xC658F
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
  std::span<BitsetChunkType const> const chunks(&CLAUSE_CAN_MATCH_ZERO[0], CLAUSE_CAN_MATCH_ZERO.size());
  return get_bit(chunks, _id);
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
 0x0, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6
};

std::array<TerminalAcceptsType const, TerminalAcceptsSize> const TERMINAL_ACCEPTS = {
 0x06, 0x0C, 0x11
};

std::array<TerminalAcceptsOffsetsType const, TerminalAcceptsOffsetsSize> const TERMINAL_ACCEPTS_OFFSETS = {
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
  size_t const start = TERMINAL_ACCEPTS_OFFSETS[state_nr_];
  size_t const end = TERMINAL_ACCEPTS_OFFSETS[state_nr_ + 1];
  return end - start;
 }

 auto get_state_final_id(StateNrType state_nr_, size_t index_) const -> FinalIdType  override {
  size_t const start = TERMINAL_ACCEPTS_OFFSETS[state_nr_];
  return TERMINAL_ACCEPTS[start + index_];
 }
};

std::array<RuleParameterDisplayNameIndirectType const, RuleParameterCount> const RULE_PARAMETER_DISPLAY_NAMES_INDIRECT = {
 0x0, 0x1, 0x3, 0x2, 0x3, 0x4, 0x5, 0x5, 0x8
};

std::array<RuleParameterIdStringIndirectType const, RuleParameterCount> const RULE_PARAMETER_ID_STRINGS_INDIRECT = {
 0x0, 0x6, 0x6, 0x2, 0x6, 0x4, 0x6, 0x6, 0x7
};

std::array<RuleParameterIdTableType const, RuleParameterIdTableSize> const RULE_PARAMETER_ID_TABLE = {
 0x0000000000000000, 0x0000000000000001, 0x0000000000000002, 0x0000000000000003, 0xFFFFFFFFFFFFFFFE
};

std::array<RuleParameterIdIndirectType const, RuleParameterCount> const RULE_PARAMETER_ID_TABLE_INDIRECT = {
 0x2, 0x3, 0x3, 0x0, 0x3, 0x1, 0x3, 0x3, 0x4
};

std::array<BitsetChunkType const, RuleParameterBooleansSize> const RULE_PARAMETER_BOOLEANS = {
 0x029, 0x100, 0x000
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
  std::span<BitsetChunkType const> const chunks(&RULE_PARAMETER_BOOLEANS[RuleParameterMergeOffset * ChunksNeeded<BitsetChunkType, RuleParameterCount>::Value], ChunksNeeded<BitsetChunkType, RuleParameterCount>::Value);
  return get_bit(chunks, _rule_id);
 }

 auto get_unpack() const -> bool override {
  std::span<BitsetChunkType const> const chunks(&RULE_PARAMETER_BOOLEANS[RuleParameterUnpackOffset * ChunksNeeded<BitsetChunkType, RuleParameterCount>::Value], ChunksNeeded<BitsetChunkType, RuleParameterCount>::Value);
  return get_bit(chunks, _rule_id);
 }

 auto get_hide() const -> bool override {
  std::span<BitsetChunkType const> const chunks(&RULE_PARAMETER_BOOLEANS[RuleParameterHideOffset * ChunksNeeded<BitsetChunkType, RuleParameterCount>::Value], ChunksNeeded<BitsetChunkType, RuleParameterCount>::Value);
  return get_bit(chunks, _rule_id);
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