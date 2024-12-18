#pragma once

#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"

namespace pmt::parserbuilder {

class RangeExpressionToFaPartFrame : public ExpressionToFaPartFrameBase {
 public:
  using ExpressionToFaPartFrameBase::ExpressionToFaPartFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;
};

}  // namespace pmt::parserbuilder