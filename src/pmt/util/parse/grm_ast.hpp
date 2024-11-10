#pragma once

#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_ast-inl.hpp"

namespace pmt::util::parse {

class GrmAst {
 public:
  enum : GenericAst::IdType {
    // Terminals
    // - Literals
    TkStringLiteral = TOKEN_STRING_LITERAL,
    TkIntegerLiteral = TOKEN_INTEGER_LITERAL,
    TkBooleanLiteral = TOKEN_BOOLEAN_LITERAL,
    // - Identifiers
    TkTerminalIdentifier = TOKEN_TERMINAL_IDENTIFIER,
    TkRuleIdentifier = TOKEN_RULE_IDENTIFIER,
    TkEpsilon = TOKEN_EPSILON,
    // - Symbols
    TkPipe = TOKEN_PIPE,
    TkSemiColon = TOKEN_SEMICOLON,
    TkEquals = TOKEN_EQUALS,
    TkComma = TOKEN_COMMA,
    TkDoubleDot = TOKEN_DOUBLE_DOT,
    // - Parenthesis
    TkOpenParen = TOKEN_OPEN_PAREN,
    TkCloseParen = TOKEN_CLOSE_PAREN,
    TkOpenBrace = TOKEN_OPEN_BRACE,
    TkCloseBrace = TOKEN_CLOSE_BRACE,
    TkOpenSquare = TOKEN_OPEN_SQUARE,
    TkCloseSquare = TOKEN_CLOSE_SQUARE,
    // - Operators
    TkPlus = TOKEN_PLUS,
    TkStar = TOKEN_STAR,
    TkQuestion = TOKEN_QUESTION,
    // - Parameter keywords
    TkKwParameterUnpack = TOKEN_KW_PARAMETER_UNPACK,
    TkKwParameterHide = TOKEN_KW_PARAMETER_HIDE,
    TkKwParameterMerge = TOKEN_KW_PARAMETER_MERGE,

    // Non-terminals
    NtGrammar = 123,  // Somewhat arbitrary, we just need it to be higher than any terminal
    NtTerminalProduction,
    NtSequence,
    NtChoices,
    NtRepetition,
    NtRepetitionRange,
    NtRange
  };

  static auto to_string(GenericAst::IdType id_) -> std::string;
};

}  // namespace pmt::util::parse