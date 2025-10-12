#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine);
PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, CharsetLiteral);

namespace pmt::parser::builder {

// The hash and eq functions rely on the following assumptions on the state machines passed as arguments:
// - They are determinized and minimized
// - They ONLY contain SymbolKindCharacter and SymbolKindHiddenCharacter transitions
enum AcceptsEqualityHandling {
 Exact,    // The accepts are considered the same for a state if they are exactly the same
 Present,  // The accepts are considered the same for a state if they both have any or none
};
auto terminal_state_machine_hash(pmt::util::sm::ct::StateMachine const& item_, AcceptsEqualityHandling accepts_equality_handling_) -> size_t;
auto terminal_state_machine_eq(pmt::util::sm::ct::StateMachine const& lhs_, pmt::util::sm::ct::StateMachine const& rhs_, AcceptsEqualityHandling accepts_equality_handling_) -> bool;

auto state_machine_from_charset_literal(pmt::parser::grammar::CharsetLiteral const& charset_literal_, pmt::util::sm::AcceptsIndexType accept_) -> pmt::util::sm::ct::StateMachine;

}  // namespace pmt::parser::builder