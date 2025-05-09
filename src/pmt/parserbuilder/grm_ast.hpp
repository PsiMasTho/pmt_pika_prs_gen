#pragma once

#include "pmt/parserbuilder/grm_ast-inl.hpp"
#include "pmt/util/smrt/generic_id.hpp"

namespace pmt::parserbuilder {

class GrmAst {
 public:
  enum : pmt::util::smrt::GenericId::IdType {
    // Terminals
    // - Literals
    TkStringLiteral = TOKEN_STRING_LITERAL,
    TkIntegerLiteral = TOKEN_INTEGER_LITERAL,
    TkBooleanLiteral = TOKEN_BOOLEAN_LITERAL,
    // - Identifiers
    TkTerminalIdentifier = TOKEN_TERMINAL_IDENTIFIER,
    TkNonterminalIdentifier = TOKEN_RULE_IDENTIFIER,
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
    TkOpenAngle = TOKEN_OPEN_ANGLE,
    TkCloseAngle = TOKEN_CLOSE_ANGLE,
    // - Operators
    TkPlus = TOKEN_PLUS,
    TkStar = TOKEN_STAR,
    TkQuestion = TOKEN_QUESTION,
    // - Parameter keywords
    TkKwParameterUnpack = TOKEN_KW_PARAMETER_UNPACK,
    TkKwParameterHide = TOKEN_KW_PARAMETER_HIDE,
    TkKwParameterMerge = TOKEN_KW_PARAMETER_MERGE,
    TkKwParameterId = TOKEN_KW_PARAMETER_ID,
    TkKwParameterCaseSensitive = TOKEN_KW_PARAMETER_CASE_SENSITIVE,
    // - Grammar properties
    TkGrammarPropertyStart = TOKEN_GRAMMAR_PROPERTY_START,
    TkGrammarPropertyWhitespace = TOKEN_GRAMMAR_PROPERTY_WHITESPACE,
    TkGrammarPropertyComment = TOKEN_GRAMMAR_PROPERTY_COMMENT,
    TkGrammarPropertyCaseSensitive = TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE,  // Also used as a parameter keyword

    // Non-terminals
    NtGrammar = 123,  // Somewhat arbitrary, we just need it to be higher than any terminal
    // - Terminal productions
    NtTerminalProduction,
    NtTerminalParameter,
    NtTerminalDefinition,
    NtTerminalSequence,
    NtTerminalChoices,
    NtTerminalRepetition,
    NtTerminalRepetitionRange,
    NtTerminalRange,
    // - Rule productions
    NtNonterminalProduction,
    NtNonterminalParameter,
    NtNonterminalDefinition,
    NtNonterminalSequence,
    NtNonterminalChoices,
    NtNonterminalRepetition,
    NtNonterminalRepetitionRange,
    // - Grammar properties
    NtGrammarProperty,
    NtGrammarPropertyWhitespace,
    NtGrammarPropertyComment,
    NtGrammarPropertyCaseSensitive,
    // - Other
    NtTerminalDefinitionPairList,
    NtTerminalDefinitionPair,
  };

  static auto id_to_string(pmt::util::smrt::GenericId::IdType id_) -> std::string;
};

}  // namespace pmt::parserbuilder