#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/rt/generic_lexer.hpp"

PMT_FW_DECL_NS_CLASS(pmt::parser::rt, ParserTablesBase);

namespace pmt::parser::rt {

class GenericParser {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  GenericLexer& _lexer;
  ParserTablesBase const& _parser_tables;
 };

 static auto parse(Args args_) -> GenericAst::UniqueHandle;
};

}  // namespace pmt::parser::rt