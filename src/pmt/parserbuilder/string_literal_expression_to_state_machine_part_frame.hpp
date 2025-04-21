#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"
#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base_captures.hpp"

namespace pmt::parserbuilder {

class StringLiteralExpressionToStateMachinePartFrame : public ExpressionToStateMachinePartFrameBase {
 public:
  using ExpressionToStateMachinePartFrameBase::ExpressionToStateMachinePartFrameBase;
  void process(ExpressionToStateMachinePartFrameBase::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;
};

}  // namespace pmt::parserbuilder
