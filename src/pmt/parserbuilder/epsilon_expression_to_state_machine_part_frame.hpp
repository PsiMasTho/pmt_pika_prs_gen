#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

namespace pmt::parserbuilder {

class EpsilonExpressionToStateMachinePartFrame : public ExpressionToStateMachinePartFrameBase {
 public:
  using ExpressionToStateMachinePartFrameBase::ExpressionToStateMachinePartFrameBase;
  void process(ExpressionToStateMachinePartFrameBase::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;

  static auto make_epsilon(pmt::util::smct::StateMachine& state_machine_) -> pmt::util::smrt::StateNrType;
};

}  // namespace pmt::parserbuilder
