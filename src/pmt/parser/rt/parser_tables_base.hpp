#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <string>

namespace pmt::parser::rt {

class ParserTablesBase {
 public:
  // -$ Types / Constants $-
  enum StateKind : uint8_t {
    StateTypeOpen,
    StateTypeAccept,
    StateTypeTerminal,  // SymbolKindTerminal & SymbolKindHiddenTerminal
    StateTypeConflict,
  };

  // -$ Functions $-
  // --$ Other $--
  // Parser tables
  virtual auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolKindType kind_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType = 0;
  virtual auto get_state_terminal_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;         // which terminals can come after this state, if any, includes hidden
  virtual auto get_state_hidden_terminal_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;  // which hidden terminals can come after this state, if any
  virtual auto get_state_conflict_transitions(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;         // which lookahead symbols can come after this state, if any
  virtual auto get_state_kind(pmt::util::sm::StateNrType state_nr_) const -> StateKind = 0;
  virtual auto get_state_accept_index(pmt::util::sm::StateNrType state_nr_) const -> size_t = 0;
  virtual auto get_eoi_accept_index() const -> size_t = 0;
  virtual auto get_accept_count() const -> size_t = 0;
  virtual auto get_conflict_count() const -> size_t = 0;

  virtual auto get_accept_index_label(size_t index_) const -> std::string;
  virtual auto get_accept_index_unpack(size_t index_) const -> bool;
  virtual auto get_accept_index_hide(size_t index_) const -> bool;
  virtual auto get_accept_index_merge(size_t index_) const -> bool;
  virtual auto get_accept_index_id(size_t index_) const -> GenericId::IdType;

  // Lookahead tables
  virtual auto get_lookahead_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType = 0;
  virtual auto get_lookahead_state_terminal_transitions(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::StateNrType state_nr_parser_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
  virtual auto get_lookahead_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
};

}  // namespace pmt::parser::rt