#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine);

namespace pmt::parser::builder {

// These functions rely on the following assumptions on the state machines passed as arguments:
// - They are determinized and minimized
// - They ONLY contain SymbolKindCharacter and SymbolKindHiddenCharacter transitions
auto terminal_state_machine_hash(pmt::util::sm::ct::StateMachine const& item_) -> size_t;
auto terminal_state_machine_eq(pmt::util::sm::ct::StateMachine const& lhs_, pmt::util::sm::ct::StateMachine const& rhs_) -> bool;

}  // namespace pmt::parser::builder