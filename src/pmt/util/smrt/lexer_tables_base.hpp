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
  virtual auto get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType = 0;
  virtual auto get_state_accepts(StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
  virtual auto get_accept_count() const -> size_t = 0;
  virtual auto get_start_accept_index() const -> size_t = 0;
  virtual auto get_eoi_accept_index() const -> size_t = 0;

  virtual auto get_accept_string(size_t index_) const -> std::string;
  virtual auto get_accept_id(size_t index_) const -> GenericId::IdType;  // Note: multiple accepts can have the same id, they should still be distinguished by index

  virtual auto id_to_string(GenericId::IdType id_) const -> std::string;

  protected:
  static auto get_state_nr_next_impl(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_, std::span<pmt::util::smrt::SymbolType const> lowers_, std::span<pmt::util::smrt::SymbolType const> uppers_, std::span<pmt::util::smrt::StateNrType const> values_, std::span<size_t const> offsets_) -> pmt::util::smrt::StateNrType;
};

}  // namespace pmt::util::smrt
