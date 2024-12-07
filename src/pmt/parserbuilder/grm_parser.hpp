#pragma once

#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/util/parse/generic_ast.hpp"

namespace pmt::parserbuilder {

class GrmParser {
 public:
  static auto parse(GrmLexer& lexer_) -> pmt::util::parse::GenericAst::UniqueHandle;
};

}  // namespace pmt::parserbuilder