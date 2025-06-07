/* Generated on: 2025-06-07 23:45:55 */
#pragma once

#include <pmt/base/bitset.hpp>
#include <pmt/util/smrt/parser_tables_base.hpp>

namespace pmt::parserbuilder {

class GrmParserTables : public pmt::util::smrt::ParserTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::util::smrt::ParserTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolKindType kind_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_hidden_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_conflict_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_state_kind(pmt::util::smrt::StateNrType state_nr_) const -> StateKind override;
  auto get_state_accept_index(pmt::util::smrt::StateNrType state_nr_) const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_count() const -> size_t override;
  auto get_conflict_count() const -> size_t override;
  auto get_accept_index_label(size_t index_) const -> std::string override;
  auto get_accept_index_unpack(size_t index_) const -> bool override;
  auto get_accept_index_hide(size_t index_) const -> bool override;
  auto get_accept_index_merge(size_t index_) const -> bool override;
  auto get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
  auto get_min_id() const -> pmt::util::smrt::GenericId::IdType override;
  auto get_id_count() const -> size_t override;
  auto get_lookahead_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_lookahead_state_terminal_transitions(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_lookahead_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
};

} // namespace pmt::parserbuilder
