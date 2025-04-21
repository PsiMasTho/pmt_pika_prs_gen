#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/generic_id.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <string>

namespace pmt::util::smrt {

class StateMachineTablesBase {
 public:
  // -$ Functions $-
  // --$ Other $--
  virtual auto get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType = 0;
  virtual auto get_state_accepts(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;

  virtual auto get_accept_count() const -> size_t = 0;
  virtual auto get_accept_string(size_t index_) const -> std::string;
  virtual auto get_accept_id(size_t index_) const -> GenericId::IdType;  // Note: multiple accepts can have the same id, they should still be distinguished by index

  virtual auto id_to_string(GenericId::IdType id_) const -> std::string;
};

}  // namespace pmt::util::smrt
