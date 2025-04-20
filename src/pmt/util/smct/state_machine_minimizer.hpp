#pragma once

#include "pmt/fw_decl.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::util::smct {

class StateMachineMinimizer {
 public:
  static void minimize(StateMachine& state_machine_);
};

}  // namespace pmt::util::smct
