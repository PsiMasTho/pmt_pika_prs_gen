#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/generic_id.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <string>

namespace pmt::util::smrt {

class LexerTablesBase {
 public:
  // -$ Functions $-
  // --$ Other $--
  // Lexer tables
  virtual auto get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType = 0;
  virtual auto get_state_terminals(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
  virtual auto get_terminal_count() const -> size_t = 0;
  virtual auto get_start_terminal_index() const -> size_t = 0;
  virtual auto get_eoi_terminal_index() const -> size_t = 0;

  virtual auto get_terminal_label(size_t index_) const -> std::string;
  virtual auto get_terminal_id(size_t index_) const -> GenericId::IdType;  // Note: multiple accepts can have the same id, they should still be distinguished by index

  virtual auto id_to_string(GenericId::IdType id_) const -> std::string;

  // Newline tables
  virtual auto get_newline_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType = 0;
  virtual auto get_newline_state_nr_accept() const -> StateNrType = 0;
};

}  // namespace pmt::util::smrt
