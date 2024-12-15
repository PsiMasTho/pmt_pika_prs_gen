#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/util/parsert/generic_lexer_tables.hpp"
#include "pmt/util/parsert/token.hpp"

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
  GenericLexer(std::string_view input_, GenericLexerTables tables_);

  // -- Tokenization --
  auto lex() -> LexReturn;
  auto lex(pmt::base::DynamicBitset const& accepts_) -> LexReturn;

 private:
  // - Data -
  GenericLexerTables _tables;
  pmt::base::DynamicBitset _accepts_valid;
  pmt::base::DynamicBitset _accepts_all;
  char const* _begin = nullptr;
  char const* _cursor = nullptr;
  char const* _end = nullptr;
};

}  // namespace pmt::util::parsert