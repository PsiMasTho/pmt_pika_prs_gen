#pragma once

#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smct/state_machine.hpp"

#include <optional>

namespace pmt::util::smct {

class StateMachinePruner {
 // -$ Types / Constants $-
 public:
  class Args {
   public:
    StateMachine& _state_machine;
    pmt::util::smrt::StateNrType _state_nr_from = pmt::util::smrt::StateNrStart;
    std::optional<pmt::util::smrt::StateNrType> _state_nr_from_new = std::nullopt;
  };

 // --$ Other $--
  static void prune(Args args_);
};

}  // namespace pmt::util::smct