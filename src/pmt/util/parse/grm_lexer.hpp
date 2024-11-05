#pragma once

#include "pmt/util/parse/generic_ast.hpp"

#include <string_view>

namespace pmt::util::parse {
class GrmLexer {
 public:
  GrmLexer(std::string_view input_);
  auto next_token() -> GenericAst::UniqueHandle;
  auto is_eof() const -> bool;

 private:
  char const* _p;
  char const* _pe;
  char const* _eof;
  char const* _ts;
  char const* _te;
  int _act;
  int _cs;
};
}  // namespace pmt::util::parse