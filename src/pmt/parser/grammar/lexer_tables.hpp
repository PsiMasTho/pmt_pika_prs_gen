/* Generated on: 2025-06-22 18:10:28 */
// clang-format off
#pragma once

#include <pmt/base/bitset.hpp>
#include <pmt/parser/rt/lexer_tables_base.hpp>

namespace pmt::parser::grammar {

class LexerTables : public pmt::parser::rt::LexerTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::parser::rt::LexerTablesBase $--
  auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto get_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_accept_count() const -> size_t override;
  auto get_start_accept_index() const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_index_display_name(size_t index_) const -> std::string override;
  auto get_accept_index_id(size_t index_) const -> pmt::parser::GenericId::IdType override;
  auto get_linecount_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
  auto is_linecount_state_nr_accepting(pmt::util::sm::SymbolValueType state_nr_) const -> bool override;
};

} // namespace pmt::parser::grammar

// clang-format on