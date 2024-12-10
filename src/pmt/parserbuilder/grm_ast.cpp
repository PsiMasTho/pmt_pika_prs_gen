#include "pmt/parserbuilder/grm_ast.hpp"

#include <stdexcept>

namespace pmt::parserbuilder {
auto GrmAst::id_to_string(pmt::util::parsert::GenericId::IdType id_) -> std::string_view {
  switch (id_) {
    case TkStringLiteral:
      return "TkStringLiteral";
    case TkIntegerLiteral:
      return "TkIntegerLiteral";
    case TkBooleanLiteral:
      return "TkBooleanLiteral";
    case TkTerminalIdentifier:
      return "TkTerminalIdentifier";
    case TkEpsilon:
      return "TkEpsilon";
    case TkPipe:
      return "TkPipe";
    case TkSemiColon:
      return "TkSemiColon";
    case TkEquals:
      return "TkEquals";
    case TkComma:
      return "TkComma";
    case TkDoubleDot:
      return "TkDoubleDot";
    case TkOpenParen:
      return "TkOpenParen";
    case TkCloseParen:
      return "TkCloseParen";
    case TkOpenBrace:
      return "TkOpenBrace";
    case TkCloseBrace:
      return "TkCloseBrace";
    case TkOpenSquare:
      return "TkOpenSquare";
    case TkCloseSquare:
      return "TkCloseSquare";
    case TkOpenAngle:
      return "TkOpenAngle";
    case TkCloseAngle:
      return "TkCloseAngle";
    case TkPlus:
      return "TkPlus";
    case TkStar:
      return "TkStar";
    case TkQuestion:
      return "TkQuestion";
    case TkKwParameterUnpack:
      return "TkKwParameterUnpack";
    case TkKwParameterHide:
      return "TkKwParameterHide";
    case TkKwParameterMerge:
      return "TkKwParameterMerge";
    // --
    case NtGrammar:
      return "NtGrammar";
    case NtTerminalProduction:
      return "NtTerminalProduction";
    case NtChoices:
      return "NtChoices";
    case NtSequence:
      return "NtSequence";
    case NtRepetition:
      return "NtRepetition";
    case NtRepetitionRange:
      return "NtRepetitionRange";
    case NtRange:
      return "NtRange";
    default:
      throw std::runtime_error("Invalid id");
  }
}
}  // namespace pmt::parserbuilder