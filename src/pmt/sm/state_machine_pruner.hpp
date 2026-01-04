#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/sm/primitives.hpp"

#include <optional>

PMT_FW_DECL_NS_CLASS(pmt::sm, StateMachine)
PMT_FW_DECL_NS_CLASS(pmt::sm, State)

namespace pmt::sm {

class StateMachinePruner {
public:
 class Args {
 public:
  StateMachine& _input_state_machine;
  StateNrType _state_nr_from = StateNrStart;
  std::optional<StateNrType> _state_nr_from_new = std::nullopt;
 };

 // -$ Functions $-
 // --$ Other $--
 static auto prune(Args args_) -> StateMachine;
};

}  // namespace pmt::sm