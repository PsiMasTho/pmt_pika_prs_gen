#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/sm/primitives.hpp"

namespace pmt::parser::rt {

class StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Other $--
 [[nodiscard]] virtual auto get_state_nr_next(pmt::sm::StateNrType state_nr_, pmt::sm::SymbolType symbol_) const -> pmt::sm::StateNrType = 0;
 [[nodiscard]] virtual auto get_state_accepts(pmt::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst = 0;
};

}  // namespace pmt::parser::rt
