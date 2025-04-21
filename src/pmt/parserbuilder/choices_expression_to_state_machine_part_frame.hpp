#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

namespace pmt::parserbuilder {

class ChoicesExpressionToStateMachinePartFrame : public ExpressionToStateMachinePartFrameBase {
 public:
  using ExpressionToStateMachinePartFrameBase::ExpressionToStateMachinePartFrameBase;
  void process(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_1(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_2(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);

  pmt::util::smct::StateMachinePart _sub_part;
  pmt::util::smct::State* _state_cur = nullptr;
  size_t _stage = 0;
  size_t _idx = 0;
};

}  // namespace pmt::parserbuilder