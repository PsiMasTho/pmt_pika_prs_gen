#include "pmt/util/parse/expression_frame_factory.hpp"

#include "pmt/util/parse/choices_expression_frame.hpp"
#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/range_expression_frame.hpp"
#include "pmt/util/parse/sequence_expression_frame.hpp"
#include "pmt/util/parse/string_literal_expression_frame.hpp"

namespace pmt::util::parse {

auto ExpressionFrameFactory::construct(AstPositionConst position_) -> ExpressionFrameBase::FrameHandle {
  switch (position_.first->get_child_at(position_.second)->get_id()) {
    case GrmAst::NtSequence:
      return std::make_shared<SequenceExpressionFrame>(position_);
    case GrmAst::NtChoices:
      return std::make_shared<ChoicesExpressionFrame>(position_);
    case GrmAst::TkStringLiteral:
      return std::make_shared<StringLiteralExpressionFrame>(position_);
    case GrmAst::NtRange:
      return std::make_shared<RangeExpressionFrame>(position_);
    default:
      throw std::runtime_error("Unknown expression frame id");
  }
}

}  // namespace pmt::util::parse