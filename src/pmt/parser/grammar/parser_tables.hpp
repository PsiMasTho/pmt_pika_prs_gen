/* Generated on: 2025-06-15 16:53:17 */
// clang-format off
#pragma once

#include <pmt/base/bitset.hpp>
#include <pmt/parser/rt/parser_tables_base.hpp>

namespace pmt::parser::grammar {

class ParserTables : public pmt::parser::rt::ParserTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::parser::rt::ParserTablesBase $--
  auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolKindType kind_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto get_state_terminal_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_hidden_terminal_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_conflict_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_kind(pmt::util::sm::StateNrType state_nr_) const -> StateKind override;
  auto get_state_accept_index(pmt::util::sm::StateNrType state_nr_) const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_count() const -> size_t override;
  auto get_conflict_count() const -> size_t override;
  auto get_accept_index_label(size_t index_) const -> std::string override;
  auto get_accept_index_unpack(size_t index_) const -> bool override;
  auto get_accept_index_hide(size_t index_) const -> bool override;
  auto get_accept_index_merge(size_t index_) const -> bool override;
  auto get_accept_index_id(size_t index_) const -> pmt::parser::GenericId::IdType override;
  auto get_lookahead_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto get_lookahead_state_terminal_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_lookahead_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
};

} // namespace pmt::parser::grammar

// clang-format on