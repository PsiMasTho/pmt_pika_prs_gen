#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
class ChoicesExpressionToFaPartFrame : public ExpressionToStateMachinePartFrameBase<TAG_> {
 public:
  using ExpressionToStateMachinePartFrameBase<TAG_>::ExpressionToStateMachinePartFrameBase;
  void process(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_1(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_2(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);

  pmt::util::parsect::StateMachinePart<TAG_> _sub_part;
  pmt::util::parsect::State<TAG_>* _state_cur = nullptr;
  size_t _stage = 0;
  size_t _idx = 0;
};

}  // namespace pmt::parserbuilder