#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"
#include "pmt/util/smct/fa_part.hpp"

namespace pmt::parserbuilder {

class SequenceExpressionToFaPartFrame : public ExpressionToStateMachinePartFrameBase<TAG_> {
 public:
  using ExpressionToStateMachinePartFrameBase::ExpressionToStateMachinePartFrameBase;
  void process(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_1(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);

  pmt::util::smct::FaPart _sub_part;
  size_t _stage = 0;
  size_t _idx = 0;
};

}  // namespace pmt::parserbuilder