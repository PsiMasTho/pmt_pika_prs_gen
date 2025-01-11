#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

#include <limits>

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
class TerminalIdentifierExpressionToStateMachinePart : public ExpressionToStateMachinePartFrameBase<TAG_> {
 public:
  using ExpressionToStateMachinePartFrameBase<TAG_>::ExpressionToStateMachinePartFrameBase;
  void process(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_) final;

 private:
  void process_stage_0(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_);
  void process_stage_1(ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_);

  std::string const* _terminal_name = nullptr;
  pmt::util::parsect::State<TAG_>* _state_reference = nullptr;
  pmt::util::parsect::State::StateNrType _state_nr_reference = std::numeric_limits<pmt::util::parsect::State::StateNrType>::max();
  size_t _stage = 0;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/terminal_identifier_expression_to_state_machine_part_frame-inl.hpp"