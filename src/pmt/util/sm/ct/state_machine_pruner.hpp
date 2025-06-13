#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <optional>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine)

namespace pmt::util::sm::ct {

class StateMachinePruner {
  // -$ Types / Constants $-
 public:
  class Args {
   public:
    StateMachine& _state_machine;
    StateNrType _state_nr_from = StateNrStart;
    std::optional<StateNrType> _state_nr_from_new = std::nullopt;
  };

  // -$ Functions $-
  // --$ Other $--
  static void prune(Args args_);
};

}  // namespace pmt::util::sm::ct