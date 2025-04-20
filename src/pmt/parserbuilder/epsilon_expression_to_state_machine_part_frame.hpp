#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

namespace pmt::parserbuilder {

template <pmt::util::smct::IsStateTag TAG_>
class EpsilonExpressionToFaPartFrame : public ExpressionToStateMachinePartFrameBase<TAG_> {
 public:
  using ExpressionToStateMachinePartFrameBase<TAG_>::ExpressionToStateMachinePartFrameBase;
  void process(ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_) final;

  static auto make_epsilon(pmt::util::smct::StateMachine& state_machine_) -> pmt::util::smct::State::StateNrType;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/epsilon_expression_to_state_machine_part_frame-inl.hpp"