#pragma once

#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/expression_frame_base.hpp"

namespace pmt::util::parse {

class ExpressionFrameFactory {
 public:
  static auto construct(AstPositionConst position_) -> ExpressionFrameBase::FrameHandle;
};

}  // namespace pmt::util::parse