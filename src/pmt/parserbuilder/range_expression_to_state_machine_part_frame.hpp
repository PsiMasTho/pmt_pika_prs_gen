#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

namespace pmt::parserbuilder {

class RangeExpressionToStateMachinePartFrame : public ExpressionToStateMachinePartFrameBase {
 public:
  using ExpressionToStateMachinePartFrameBase::ExpressionToStateMachinePartFrameBase;
  void process(ExpressionToStateMachinePartFrameBase::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;
};

}  // namespace pmt::parserbuilder
