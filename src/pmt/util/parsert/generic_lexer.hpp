#pragma once

#include "pmt/base/bitset.hpp"
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
  auto lex(pmt::base::Bitset const& accepts_) -> LexReturn;

 private:
  // - Data -
  GenericLexerTables _tables;
  pmt::base::Bitset _accepts_valid;
  pmt::base::Bitset _accepts_all;
  char const* _buffer = nullptr;
  size_t _buffer_size = 0;
  size_t _cursor = 0;
};

}  // namespace pmt::util::parsert