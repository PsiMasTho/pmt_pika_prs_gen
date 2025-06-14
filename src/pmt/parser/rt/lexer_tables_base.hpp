#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <string>

namespace pmt::parser::rt {

class LexerTablesBase {
 public:
  // -$ Functions $-
  // --$ Other $--
  // Lexer tables
  virtual auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType = 0;
  virtual auto get_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
  virtual auto get_accept_count() const -> size_t = 0;
  virtual auto get_start_accept_index() const -> size_t = 0;
  virtual auto get_eoi_accept_index() const -> size_t = 0;

  virtual auto get_accept_index_label(size_t index_) const -> std::string;
  virtual auto get_accept_index_id(size_t index_) const -> GenericId::IdType;  // Note: multiple accepts can have the same id, they should still be distinguished by index

  // Linecount tables
  virtual auto get_linecount_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType = 0;
  virtual auto is_linecount_state_nr_accepting(pmt::util::sm::StateNrType state_nr_) const -> bool = 0;
};

}  // namespace pmt::parser::rt
