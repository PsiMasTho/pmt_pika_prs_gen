#pragma once

#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_id-inl.hpp"

namespace pmt::util::parse {

class GrmId {
 public:
  enum : GenericAst::IdType {
    // Terminals
    TkStringLiteral = TOKEN_STRING_LITERAL,
    TkIdentifier = TOKEN_IDENTIFIER,
    TkPipe = TOKEN_PIPE,
    TkSemiColon = TOKEN_SEMICOLON,
    TkEquals = TOKEN_EQUALS,

    // Non-terminals
    NtGrammar = 6,
    NtDefinition,
    NtChoices,
    NtSequence
  };

  static auto to_string(GenericAst::IdType id_) -> std::string;
};

}  // namespace pmt::util::parse