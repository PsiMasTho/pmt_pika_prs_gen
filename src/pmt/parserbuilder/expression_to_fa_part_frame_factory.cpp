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
#include "pmt/util/parsert/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::parsert;

auto ExpressionToFaPartFrameFactory::construct(GenericAst const& ast_, GenericAstPath const& path_) -> ExpressionToFaPartFrameBase::FrameHandle {
  GenericId::IdType const id = path_.resolve(ast_)->get_id();
  switch (id) {
    case GrmAst::NtTerminalSequence:
      return std::make_shared<SequenceExpressionToFaPartFrame>(path_);
    case GrmAst::NtTerminalChoices:
      return std::make_shared<ChoicesExpressionToFaPartFrame>(path_);
    case GrmAst::NtTerminalRepetition:
      return std::make_shared<RepetitionExpressionToFaPartFrame>(path_);
    case GrmAst::TkStringLiteral:
      return std::make_shared<StringLiteralExpressionToFaPartFrame>(path_);
    case GrmAst::NtTerminalRange:
      return std::make_shared<RangeExpressionToFaPartFrame>(path_);
    case GrmAst::TkIntegerLiteral:
      return std::make_shared<IntegerLiteralExpressionToFaPartFrame>(path_);
    case GrmAst::TkEpsilon:
      return std::make_shared<EpsilonExpressionToFaPartFrame>(path_);
    case GrmAst::TkTerminalIdentifier:
      return std::make_shared<TerminalIdentifierExpressionToFaPart>(path_);
    case GrmAst::NtTerminalDefinition:
      return construct(ast_, path_.clone_push(0));
    default: {
      throw std::runtime_error("Unknown expression frame id: " + GrmAst::id_to_string(id));
    }
  }
}

}  // namespace pmt::parserbuilder