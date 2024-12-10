#pragma once

#include "pmt/util/parsert/generic_lexer_tables.hpp"
#include "pmt/util/parsert/token.hpp"

#include <memory>
#include <string_view>

namespace pmt::util::parsert {

class GenericLexer {
 public:
  // - Types -
  class LexReturn {
   public:
    Token _token;
    GenericLexerTables::TableIndexType _accepted;
  };

  // - Member functions -
  // -- Lifetime --
  GenericLexer(std::string_view input_, GenericLexerTables const& tables_);

  // -- Tokenization --
  auto lex() -> LexReturn;
  auto lex(GenericLexerTables::TableIndexType const* accepts_) -> LexReturn;

 private:
  // - Nonstatic member functions -
  void fill_bitset(GenericLexerTables::TableIndexType* bitset_, bool value_);

  // - Data -
  std::unique_ptr<GenericLexerTables::TableIndexType[]> _accepts_valid;
  std::unique_ptr<GenericLexerTables::TableIndexType[]> _accepts_all;
  GenericLexerTables const& _tables;
  char const* _begin = nullptr;
  char const* _cursor = nullptr;
  char const* _end = nullptr;
};

}  // namespace pmt::util::parsert