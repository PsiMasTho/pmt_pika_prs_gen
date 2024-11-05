#pragma once

#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/parse/grm_lexer.hpp"

namespace pmt::util::parse {

class GrmParser {
 public:
  static auto parse(GrmLexer& lexer_) -> GenericAst::UniqueHandle;
};

}  // namespace pmt::util::parse