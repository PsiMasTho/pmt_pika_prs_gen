#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"
#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base_captures.hpp"

namespace pmt::parserbuilder {

template <pmt::util::smct::IsStateTag TAG_>
class StringLiteralExpressionToStateMachinePartFrame : public ExpressionToStateMachinePartFrameBase<TAG_> {
 public:
  using ExpressionToStateMachinePartFrameBase<TAG_>::ExpressionToStateMachinePartFrameBase;
  void process(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_) final;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/string_literal_expression_to_state_machine_part_frame-inl.hpp"