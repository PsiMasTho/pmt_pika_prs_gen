#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/smct/state.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::util::smct {

class StateMachinePruner {
 public:
  static void prune(StateMachine& state_machine_, State::StateNrType state_nr_from_ = 0, State::StateNrType state_nr_from_new_ = 0, bool renumber_ = false);
};

}  // namespace pmt::util::smct