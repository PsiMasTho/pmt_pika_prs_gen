#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <unordered_map>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine)

namespace pmt::parser::builder {

class LookaheadTerminalTransitionMaskBuilder {
 public:
  // -$ Types / Constants $-
  class Args {
   public:
    pmt::util::sm::ct::StateMachine const& _parser_state_machine;
    pmt::util::sm::ct::StateMachine const& _lookahead_state_machine;
  };

  // -$ Functions $-
  // --$ Other $--
  static auto build(Args args_) -> std::unordered_map<pmt::util::sm::StateNrType, std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset>>;
};

}  // namespace pmt::parser::builder