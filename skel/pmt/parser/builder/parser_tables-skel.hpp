/* Generated on: /* $replace TIMESTAMP */ */
// clang-format off
#pragma once

#include <pmt/base/bitset.hpp>
#include <pmt/parser/rt/parser_tables_base.hpp>

/* $replace NAMESPACE_OPEN */

class /* $replace CLASS_NAME */ : public pmt::parser::rt::ParserTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::parser::rt::ParserTablesBase $--
  auto get_state_nr_next(pmt::sm::StateNrType state_nr_, SymbolKindType kind_, pmt::sm::SymbolType symbol_) const -> pmt::sm::StateNrType override;
  auto get_state_terminal_transitions(pmt::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_hidden_terminal_transitions(pmt::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_conflict_transitions(pmt::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_kind(pmt::sm::StateNrType state_nr_) const -> StateKind override;
  auto get_state_accept_index(pmt::sm::StateNrType state_nr_) const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_count() const -> size_t override;
  auto get_conflict_count() const -> size_t override;
  auto get_accept_index_display_name(size_t index_) const -> std::string override;
  auto get_accept_index_unpack(size_t index_) const -> bool override;
  auto get_accept_index_hide(size_t index_) const -> bool override;
  auto get_accept_index_merge(size_t index_) const -> bool override;
  auto get_accept_index_id(size_t index_) const -> pmt::parser::GenericId::IdType override;
  auto get_lookahead_state_nr_next(pmt::sm::StateNrType state_nr_, pmt::sm::SymbolType symbol_) const -> pmt::sm::StateNrType override;
  auto get_lookahead_state_terminal_transitions(pmt::sm::StateNrType state_nr_, pmt::sm::StateNrType state_nr_parser_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_lookahead_state_accepts(pmt::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
};

/* $replace NAMESPACE_CLOSE */
// clang-format on