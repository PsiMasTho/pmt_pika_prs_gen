#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/generic_lexer_tables.hpp"
#include "pmt/util/smrt/token.hpp"

#include <string_view>

namespace pmt::util::smrt {

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

}  // namespace pmt::util::smrt