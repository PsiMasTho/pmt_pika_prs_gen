#pragma once

#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"
#include "pmt/parserbuilder/fa_part.hpp"

namespace pmt::parserbuilder {

class SequenceExpressionToFaPartFrame : public ExpressionToFaPartFrameBase {
 public:
  using ExpressionToFaPartFrameBase::ExpressionToFaPartFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, Captures& captures_);
  void process_stage_1(CallstackType& callstack_, Captures& captures_);

  FaPart _sub_part;
  size_t _stage = 0;
  size_t _idx = 0;
};

}  // namespace pmt::parserbuilder