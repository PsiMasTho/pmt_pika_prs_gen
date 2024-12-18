#include "pmt/parserbuilder/expression_to_fa_part_frame_factory.hpp"

#include "pmt/parserbuilder/choices_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/epsilon_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/integer_literal_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/range_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/repetition_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/sequence_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/string_literal_expression_to_fa_part_frame.hpp"
#include "pmt/parserbuilder/terminal_identifier_expression_to_fa_part_frame.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsert;

auto ExpressionToFaPartFrameFactory::construct(GenericAst::PositionConst position_) -> ExpressionToFaPartFrameBase::FrameHandle {
  GenericId::IdType const id = position_.first->get_child_at(position_.second)->get_id();
  switch (id) {
    case GrmAst::NtTerminalSequence:
      return std::make_shared<SequenceExpressionToFaPartFrame>(position_);
    case GrmAst::NtTerminalChoices:
      return std::make_shared<ChoicesExpressionToFaPartFrame>(position_);
    case GrmAst::NtRepetition:
      return std::make_shared<RepetitionExpressionToFaPartFrame>(position_);
    case GrmAst::TkStringLiteral:
      return std::make_shared<StringLiteralExpressionToFaPartFrame>(position_);
    case GrmAst::NtRange:
      return std::make_shared<RangeExpressionToFaPartFrame>(position_);
    case GrmAst::TkIntegerLiteral:
      return std::make_shared<IntegerLiteralExpressionToFaPartFrame>(position_);
    case GrmAst::TkEpsilon:
      return std::make_shared<EpsilonExpressionToFaPartFrame>(position_);
    case GrmAst::TkTerminalIdentifier:
      return std::make_shared<TerminalIdentifierExpressionToFaPart>(position_);
    default: {
      throw std::runtime_error("Unknown expression frame id: " + GrmAst::id_to_string(id));
    }
  }
}

}  // namespace pmt::parserbuilder