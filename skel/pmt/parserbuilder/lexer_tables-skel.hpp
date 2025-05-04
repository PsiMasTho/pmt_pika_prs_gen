/* $replace TIMESTAMP */
#pragma once

#include <pmt/base/bitset.hpp>
#include <pmt/util/smrt/lexer_tables_base.hpp>

/* $replace NAMESPACE_OPEN */

class /* $replace CLASS_NAME */ : public pmt::util::smrt::LexerTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::util::smrt::LexerTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_state_terminals(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_terminal_count() const -> size_t override;
  auto get_start_terminal_index() const -> size_t override;
  auto get_eoi_terminal_index() const -> size_t override;
  auto get_terminal_label(size_t index_) const -> std::string override;
  auto get_terminal_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
};

/* $replace NAMESPACE_CLOSE */