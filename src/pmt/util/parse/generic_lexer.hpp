#pragma once

#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/generic_lexer_tables.hpp"

#include <span>
#include <string_view>

namespace pmt::util::parse {

class GenericLexer {
 public:
  GenericLexer(std::string_view input_, GenericLexerTables const& tables_);

  auto next_token(std::span<uint64_t const> accepts_) -> GenericAst::UniqueHandle;

 private:
  char const* _begin = nullptr;
  char const* _cursor = nullptr;
  char const* _end = nullptr;
  GenericLexerTables const& _tables;
};

}  // namespace pmt::util::parse