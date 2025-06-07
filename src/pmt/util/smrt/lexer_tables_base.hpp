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
  virtual auto get_state_nr_next(StateNrType state_nr_, SymbolValueType symbol_) const -> StateNrType = 0;
  virtual auto get_state_accepts(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
  virtual auto get_accept_count() const -> size_t = 0;
  virtual auto get_start_accept_index() const -> size_t = 0;
  virtual auto get_eoi_accept_index() const -> size_t = 0;

  virtual auto get_accept_index_label(size_t index_) const -> std::string;
  virtual auto get_accept_index_id(size_t index_) const -> GenericId::IdType;  // Note: multiple accepts can have the same id, they should still be distinguished by index

  virtual auto id_to_string(GenericId::IdType id_) const -> std::string;
  virtual auto get_min_id() const -> GenericId::IdType = 0;
  virtual auto get_id_count() const -> size_t = 0;

  // Linecount tables
  virtual auto get_linecount_state_nr_next(StateNrType state_nr_, SymbolValueType symbol_) const -> StateNrType = 0;
  virtual auto is_linecount_state_nr_accepting(StateNrType state_nr_) const -> bool = 0;
};

}  // namespace pmt::util::smrt
