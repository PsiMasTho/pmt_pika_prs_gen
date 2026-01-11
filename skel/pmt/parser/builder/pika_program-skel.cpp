/* Generated on: /* $replace TIMESTAMP */ */
// clang-format off
#include "/* $replace HEADER_INCLUDE_PATH */"

#include <pmt/parser/rt/state_machine_tables_base.hpp>

#include <algorithm>
#include <array>
#include <cassert>

/* $replace NAMESPACE_OPEN */
using namespace pmt::parser;
using namespace pmt::parser::rt;
using namespace pmt::base;
using namespace pmt::sm;

namespace {

using TerminalTransitionsType = /* $replace TERMINAL_TRANSITIONS_TYPE */;
using TerminalTransitionsOffsetsType = /* $replace TERMINAL_TRANSITIONS_OFFSETS_TYPE */;
using ClauseChildIdsOffsetsType = /* $replace CLAUSE_CHILD_IDS_OFFSETS_TYPE */;
using ClauseChildIdsType = /* $replace CLAUSE_CHILD_IDS_TYPE */;
using ClauseSeedParentIdsOffsetsType = /* $replace CLAUSE_SEED_PARENT_IDS_OFFSETS_TYPE */;
using ClauseSeedParentIdsType = /* $replace CLAUSE_SEED_PARENT_IDS_TYPE */;
using ClauseSpecialIdType = /* $replace CLAUSE_SPECIAL_ID_TYPE */;


template <size_t BITCOUNT_>
class ChunksNeeded {
public:
 inline static constexpr size_t Value = (BITCOUNT_ + Bitset::ChunkBit - 1) / Bitset::ChunkBit;
};

enum : size_t {
 TerminalStateCount = /* $replace TERMINAL_STATE_COUNT */,
 TerminalTransitionsSize = TerminalStateCount * 3, // lowers, uppers, values
 TerminalTransitionsOffsetsSize = TerminalStateCount + 1,
 TerminalAcceptsChunkCount = /* $replace TERMINAL_ACCEPTS_CHUNK_COUNT */,
 TerminalAcceptsSize = TerminalStateCount * TerminalAcceptsChunkCount,
 ClauseCount = /* $replace CLAUSE_COUNT */,
 ClauseChildIdsSize = /* $replace CLAUSE_CHILD_IDS_SIZE */,
 ClauseChildIdsOffsetsSize = ClauseCount + 1,
 ClauseSeedParentIdsSize = /* $replace CLAUSE_SEED_PARENT_IDS_SIZE */,
 ClauseSeedParentIdsOffsetsSize = ClauseCount + 1,
 ClauseCanMatchZeroSize = ChunksNeeded<ClauseCount>::Value,
 RuleParameterCount = /* $replace RULE_PARAMETER_COUNT */,
 RuleParameterMergeOffset = 0,
 RuleParameterUnpackOffset = 1,
 RuleParameterHideOffset = 2,
 RuleParameterBooleansSize = ChunksNeeded<RuleParameterCount>::Value * 3, // merge, unpack, hide
};

auto get_bit(Bitset::ChunkSpanConst const& chunks_, size_t idx_) -> bool {
 size_t const chunk_idx = idx_ / Bitset::ChunkBit;
 size_t const bit_idx = idx_ % Bitset::ChunkBit;
 return (chunks_[chunk_idx] >> bit_idx) & 1;
}

std::array<ClauseBase::Tag, ClauseCount> const CLAUSE_TAGS = {
/* $replace CLAUSE_TAGS */
};

std::array<ClauseChildIdsType, ClauseChildIdsSize> const CLAUSE_CHILD_IDS = {
/* $replace CLAUSE_CHILD_IDS */
};

std::array<ClauseChildIdsOffsetsType, ClauseChildIdsOffsetsSize> const CLAUSE_CHILD_IDS_OFFSETS = {
/* $replace CLAUSE_CHILD_IDS_OFFSETS */
};

std::array<ClauseSeedParentIdsType, ClauseSeedParentIdsSize> const CLAUSE_SEED_PARENT_IDS = {
/* $replace CLAUSE_SEED_PARENT_IDS */
};

std::array<ClauseSeedParentIdsOffsetsType, ClauseSeedParentIdsOffsetsSize> const CLAUSE_SEED_PARENT_IDS_OFFSETS = {
/* $replace CLAUSE_SEED_PARENT_IDS_OFFSETS */
};

std::array<ClauseSpecialIdType, ClauseCount> const CLAUSE_SPECIAL_IDS = {
/* $replace CLAUSE_SPECIAL_IDS */
};

std::array<Bitset::ChunkType const, ClauseCanMatchZeroSize> const CLAUSE_CAN_MATCH_ZERO = {
/* $replace CLAUSE_CAN_MATCH_ZERO */
};

class Clause : public ClauseBase {
 // -$ Types / Constants $-
 // -$ Data $-
 ClauseBase::IdType _id = 0;

public:
 // -$ Functions $-
 // --$ Inherited: pmt::parser::rt::ClauseBase $--
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
  return get_bit(CLAUSE_CAN_MATCH_ZERO, _id);
 }

 // --$ Other $--
 void set_id(ClauseBase::IdType id_) {
  _id = id_;
 }
};

std::array<TerminalTransitionsType, TerminalTransitionsSize> const TERMINAL_TRANSITIONS = {
/* $replace TERMINAL_TRANSITIONS */
};

std::array<TerminalTransitionsOffsetsType, TerminalTransitionsOffsetsSize> const TERMINAL_TRANSITIONS_OFFSETS = {
/* $replace TERMINAL_TRANSITIONS_OFFSETS */
};

std::array<Bitset::ChunkType const, TerminalAcceptsSize> const TERMINAL_ACCEPTS = {
/* $replace TERMINAL_ACCEPTS */
};

class TerminalTables : public StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Inherited: pmt::parser::rt::StateMachineTablesBase $--
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

 auto get_state_accepts(StateNrType state_nr_) const -> Bitset::ChunkSpanConst override {
  return Bitset::ChunkSpanConst(TERMINAL_ACCEPTS.begin() + state_nr_ * TerminalAcceptsChunkCount, TerminalAcceptsChunkCount);
 }
};

std::array<char const* const, RuleParameterCount> const RULE_PARAMETER_DISPLAY_NAMES = {
/* $replace RULE_PARAMETER_DISPLAY_NAMES */
};

std::array<char const* const, RuleParameterCount> const RULE_PARAMETER_ID_STRINGS = {
/* $replace RULE_PARAMETER_ID_STRINGS */
};

std::array<GenericId::IdType, RuleParameterCount> const RULE_PARAMETER_ID_VALUES = {
/* $replace RULE_PARAMETER_ID_VALUES */
};

std::array<Bitset::ChunkType const, RuleParameterBooleansSize> const RULE_PARAMETER_BOOLEANS = {
/* $replace RULE_PARAMETER_BOOLEANS */
};

class RuleParameters : public RuleParametersBase {
 // -$ Data $-
 size_t _rule_id;

public:
 // -$ Functions $-
 // --$ Inherited: pmt::parser::rt::RuleParametersBase $--
 auto get_display_name() const -> std::string_view override {
  return RULE_PARAMETER_DISPLAY_NAMES[_rule_id];
 }

 auto get_id_string() const -> std::string_view override {
  return RULE_PARAMETER_ID_STRINGS[_rule_id];
 }

 auto get_id_value() const -> GenericId::IdType override {
  return RULE_PARAMETER_ID_VALUES[_rule_id];
 } 

 auto get_merge() const -> bool override {
  return get_bit(Bitset::ChunkSpanConst(&RULE_PARAMETER_BOOLEANS[RuleParameterMergeOffset * ChunksNeeded<RuleParameterCount>::Value], ChunksNeeded<RuleParameterCount>::Value), _rule_id);
 }

 auto get_unpack() const -> bool override {
  return get_bit(Bitset::ChunkSpanConst(&RULE_PARAMETER_BOOLEANS[RuleParameterUnpackOffset * ChunksNeeded<RuleParameterCount>::Value], ChunksNeeded<RuleParameterCount>::Value), _rule_id);
 }

 auto get_hide() const -> bool override {
  return get_bit(Bitset::ChunkSpanConst(&RULE_PARAMETER_BOOLEANS[RuleParameterHideOffset * ChunksNeeded<RuleParameterCount>::Value], ChunksNeeded<RuleParameterCount>::Value), _rule_id);
 }

 // --$ Other $--
 void set_rule_id(size_t rule_id_) {
  _rule_id = rule_id_;
 }
};

} // namespace

auto /* $replace CLASS_NAME */::fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& {
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

auto /* $replace CLASS_NAME */::fetch_rule_parameters(ClauseBase::IdType rule_id_) const -> RuleParametersBase const& {
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