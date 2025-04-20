#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

namespace pmt::parserbuilder {

template <util::smct::IsStateTag TAG_>
class ExpressionToStateMachinePartFrameFactory {
 public:
  static auto construct(pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) -> ExpressionToStateMachinePartFrameBase<TAG_>::FrameHandle;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_factory-inl.hpp"