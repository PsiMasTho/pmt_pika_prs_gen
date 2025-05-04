#pragma once

#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <string>

namespace pmt::util::smrt {

 class ParserTablesBase {
  public:
   // -$ Functions $-
   // --$ Other $--
   virtual auto get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType = 0;
   virtual auto get_state_nonterminal(StateNrType state_nr_) const -> size_t;
 
   virtual auto get_nonterminal_label(size_t index_) const -> std::string;
   virtual auto get_nonterminal_id(size_t index_) const -> GenericId::IdType;
   virtual auto id_to_string(GenericId::IdType id_) const -> std::string;
 };

}