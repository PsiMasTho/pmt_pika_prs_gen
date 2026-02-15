#pragma once

#include "pmt/pika/rt/primitives.hpp"

#include <cstddef>

namespace pmt::pika::rt {

class StateMachineTablesBase {
public:
 // -$ Functions $-
 // --$ Virtual $--
 [[nodiscard]] virtual auto get_state_nr_next(StateNrType state_nr_, SymbolType symbol_) const -> StateNrType = 0;

 [[nodiscard]] virtual auto get_state_final_id_count(StateNrType state_nr_) const -> size_t = 0;
 [[nodiscard]] virtual auto get_state_final_id(StateNrType state_nr_, size_t index_) const -> IdType = 0;
};

}  // namespace pmt::pika::rt
