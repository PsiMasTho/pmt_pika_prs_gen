#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <cstddef>
#include <functional>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine);
PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, CharsetLiteral);
PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, Grammar);
PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, RuleExpression);

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

using RuleNameToSymbolFnType = std::function<pmt::util::sm::SymbolValueType(std::string const&)>;

auto state_machine_from_regular_rule(pmt::parser::grammar::Grammar const& grammar_, pmt::parser::grammar::RuleExpression const& regular_rule_expression_, pmt::util::sm::AcceptsIndexType accept_, RuleNameToSymbolFnType const& rule_name_to_symbol_fn_) -> pmt::util::sm::ct::StateMachine;

}  // namespace pmt::parser::builder