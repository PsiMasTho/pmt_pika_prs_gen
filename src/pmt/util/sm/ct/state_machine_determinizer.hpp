#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/sm/primitives.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine)

namespace pmt::util::sm::ct {

class StateMachineDeterminizer {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  StateMachine& _state_machine;
  StateNrType _state_nr_from = StateNrStart;
 };

 // -$ Functions $-
 // --$ Other $--
 static void determinize(Args args_);
};

}  // namespace pmt::util::sm::ct