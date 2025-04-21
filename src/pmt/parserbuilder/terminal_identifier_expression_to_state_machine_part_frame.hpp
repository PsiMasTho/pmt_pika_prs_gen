#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

#include <limits>

namespace pmt::parserbuilder {

class TerminalIdentifierExpressionToStateMachinePart : public ExpressionToStateMachinePartFrameBase {
 public:
  using ExpressionToStateMachinePartFrameBase::ExpressionToStateMachinePartFrameBase;
  void process(ExpressionToStateMachinePartFrameBase::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;

 private:
  void process_stage_0(ExpressionToStateMachinePartFrameBase::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_1(ExpressionToStateMachinePartFrameBaseCaptures& captures_);

  std::string const* _terminal_name = nullptr;
  pmt::util::smct::State* _state_reference = nullptr;
  pmt::util::smrt::StateNrType _state_nr_reference = std::numeric_limits<pmt::util::smrt::StateNrType>::max();
  size_t _stage = 0;
};

}  // namespace pmt::parserbuilder
