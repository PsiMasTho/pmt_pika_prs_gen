#pragma once

#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"

namespace pmt::parserbuilder {

class ExpressionToFaPartFrameFactory {
 public:
  static auto construct(pmt::util::parsert::GenericAst::PositionConst position_) -> ExpressionToFaPartFrameBase::FrameHandle;
};

}  // namespace pmt::parserbuilder