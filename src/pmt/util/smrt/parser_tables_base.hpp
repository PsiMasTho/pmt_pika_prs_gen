#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <string>

namespace pmt::util::smrt {

 class ParserTablesBase {
  public:
   // -$ Types / Constants $-
   enum StateKind : uint8_t {
    StateTypeOpen,
    StateTypeAccept,
    StateTypeTerminal, // SymbolKindTerminal & SymbolKindHiddenTerminal
    StateTypeConflict,
   };

   // -$ Functions $-
   // --$ Other $--
   // Parser tables
   virtual auto get_state_nr_next(StateNrType state_nr_, SymbolKindType kind_, SymbolValueType symbol_) const -> StateNrType = 0;
   virtual auto get_state_terminal_transitions(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0; // which terminals can come after this state, if any, includes hidden
   virtual auto get_state_hidden_terminal_transitions(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0; // which hidden terminals can come after this state, if any
   virtual auto get_state_conflict_transitions(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0; // which lookahead symbols can come after this state, if any
   virtual auto get_state_kind(StateNrType state_nr_) const -> StateKind = 0;
   virtual auto get_state_accept_index(StateNrType state_nr_) const -> size_t = 0;
   virtual auto get_eoi_accept_index() const -> size_t = 0;
   virtual auto get_accept_count() const -> size_t = 0;
   virtual auto get_conflict_count() const -> size_t = 0;
 
   virtual auto get_accept_index_label(size_t index_) const -> std::string;
   virtual auto get_accept_index_unpack(size_t index_) const -> bool;
   virtual auto get_accept_index_hide(size_t index_) const -> bool;
   virtual auto get_accept_index_merge(size_t index_) const -> bool;
   virtual auto get_accept_index_id(size_t index_) const -> GenericId::IdType;
   virtual auto id_to_string(GenericId::IdType id_) const -> std::string;
   virtual auto get_min_id() const -> GenericId::IdType = 0;
   virtual auto get_id_count() const -> size_t = 0;

   // Lookahead tables
   virtual auto get_lookahead_state_nr_next(StateNrType state_nr_, SymbolValueType symbol_) const -> StateNrType = 0;
   virtual auto get_lookahead_state_terminal_transitions(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
   virtual auto get_lookahead_state_accepts(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
 };

}