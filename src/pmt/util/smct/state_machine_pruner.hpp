#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <optional>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::util::smct {

class StateMachinePruner {
 public:
  static void prune(StateMachine& state_machine_, pmt::util::smrt::StateNrType state_nr_from_ = pmt::util::smrt::StateNrStart, std::optional<pmt::util::smrt::StateNrType> state_nr_from_new_ = std::nullopt);
};

}  // namespace pmt::util::smct