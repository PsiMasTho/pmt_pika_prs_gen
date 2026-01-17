#pragma once

#include "pmt/sm/primitives.hpp"

namespace pmt::parser::rt {

class StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Other $--
 [[nodiscard]] virtual auto get_state_nr_next(pmt::sm::StateNrType state_nr_, pmt::sm::SymbolType symbol_) const -> pmt::sm::StateNrType = 0;

 [[nodiscard]] virtual auto get_state_accept_count(pmt::sm::StateNrType state_nr_) const -> size_t = 0;
 [[nodiscard]] virtual auto get_state_accept_at(pmt::sm::StateNrType state_nr_, size_t index_) const -> pmt::sm::AcceptsIndexType = 0;
};

}  // namespace pmt::parser::rt
