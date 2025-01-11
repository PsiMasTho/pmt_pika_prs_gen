#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"

namespace pmt::parserbuilder {

template <util::parsect::IsStateTag TAG_>
class ExpressionToStateMachinePartFrameFactory {
 public:
  static auto construct(pmt::util::parsert::GenericAst const& ast_, pmt::util::parsert::GenericAstPath const& path_) -> ExpressionToStateMachinePartFrameBase<TAG_>::FrameHandle;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_factory-inl.hpp"