#include "pmt/parserbuilder/expression_to_state_machine_part_frame_factory.hpp"

#include "pmt/parserbuilder/choices_expression_to_state_machine_part_frame.hpp"
#include "pmt/parserbuilder/epsilon_expression_to_state_machine_part_frame.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/integer_literal_expression_to_state_machine_part_frame.hpp"
#include "pmt/parserbuilder/range_expression_to_state_machine_part_frame.hpp"
#include "pmt/parserbuilder/repetition_expression_to_state_machine_part_frame.hpp"
#include "pmt/parserbuilder/sequence_expression_to_state_machine_part_frame.hpp"
#include "pmt/parserbuilder/string_literal_expression_to_state_machine_part_frame.hpp"
#include "pmt/parserbuilder/terminal_identifier_expression_to_state_machine_part_frame.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

namespace pmt::parserbuilder {

auto ExpressionToStateMachinePartFrameFactory::construct(pmt::util::smrt::GenericAst const& ast_, pmt::util::smrt::GenericAstPath const& path_) -> ExpressionToStateMachinePartFrameBase::FrameHandle {
  pmt::util::smrt::GenericId::IdType const id = path_.resolve(ast_)->get_id();
  switch (id) {
    case GrmAst::NtTerminalSequence:
      return std::make_shared<SequenceExpressionToStateMachinePartFrame>(path_);
    case GrmAst::NtTerminalChoices:
      return std::make_shared<ChoicesExpressionToStateMachinePartFrame>(path_);
    case GrmAst::NtTerminalRepetition:
      return std::make_shared<RepetitionExpressionToStateMachinePartFrame>(path_);
    case GrmAst::TkStringLiteral:
      return std::make_shared<StringLiteralExpressionToStateMachinePartFrame>(path_);
    case GrmAst::NtTerminalRange:
      return std::make_shared<RangeExpressionToStateMachinePartFrame>(path_);
    case GrmAst::TkIntegerLiteral:
      return std::make_shared<IntegerLiteralExpressionToStateMachinePartFrame>(path_);
    case GrmAst::TkEpsilon:
      return std::make_shared<EpsilonExpressionToStateMachinePartFrame>(path_);
    case GrmAst::TkTerminalIdentifier:
      return std::make_shared<TerminalIdentifierExpressionToStateMachinePart>(path_);
    case GrmAst::NtTerminalDefinition:
      return construct(ast_, path_.clone_push(0));
    default: {
      throw std::runtime_error("Unknown expression frame id: " + GrmAst::id_to_string(id));
    }
  }
}

}  // namespace pmt::parserbuilder