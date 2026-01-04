#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/sm/primitives.hpp"

namespace pmt::parser::rt {

class StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Other $--
 virtual auto get_state_nr_next(pmt::sm::StateNrType state_nr_, pmt::sm::SymbolType symbol_) const -> pmt::sm::StateNrType = 0;
 virtual auto get_state_accepts(pmt::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
 virtual auto get_accept_count() const -> size_t = 0;
};

}  // namespace pmt::parser::rt
