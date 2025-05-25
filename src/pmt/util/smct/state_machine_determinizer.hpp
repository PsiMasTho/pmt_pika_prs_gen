#pragma once

#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smct/state_machine.hpp"


namespace pmt::util::smct {

class StateMachineDeterminizer {
 public:
  // -$ Types / Constants $-
  class Args {
   public:
    StateMachine& _state_machine;
    pmt::util::smrt::StateNrType _state_nr_from = pmt::util::smrt::StateNrStart;
  };

 // --$ Other $--
  static void determinize(Args args_);
};

}  // namespace pmt::util::smct