#pragma once

#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"

namespace pmt::parserbuilder {

class ExpressionToFaPartFrameFactory {
 public:
  static auto construct(pmt::util::parsert::GenericAst const& ast_, pmt::util::parsert::GenericAstPath const& path_) -> ExpressionToFaPartFrameBase::FrameHandle;
};

}  // namespace pmt::parserbuilder