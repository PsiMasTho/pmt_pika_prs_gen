/* Generated on: 2025-06-07 22:39:25 */
#pragma once

#include <pmt/base/bitset.hpp>
#include <pmt/util/smrt/lexer_tables_base.hpp>

namespace pmt::parserbuilder {

class LexerClass : public pmt::util::smrt::LexerTablesBase {
 public:
  // -$ Functions $-
  // --$ Inherited: pmt::util::smrt::LexerTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto get_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
  auto get_accept_count() const -> size_t override;
  auto get_start_accept_index() const -> size_t override;
  auto get_eoi_accept_index() const -> size_t override;
  auto get_accept_index_label(size_t index_) const -> std::string override;
  auto get_accept_index_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;
  auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;
  auto get_min_id() const -> pmt::util::smrt::GenericId::IdType override;
  auto get_id_count() const -> size_t override;
  auto get_linecount_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolValueType symbol_) const -> pmt::util::smrt::StateNrType override;
  auto is_linecount_state_nr_accepting(pmt::util::smrt::SymbolValueType state_nr_) const -> bool override;
};

} // namespace pmt::parserbuilder
