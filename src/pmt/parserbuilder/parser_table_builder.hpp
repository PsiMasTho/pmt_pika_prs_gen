#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/parser_tables.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst);
PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, GrammarData);
PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, LexerTables);

namespace pmt::parserbuilder {

class ParserTableBuilder {
 public:
  // -$ Types / Constants $-
  class Args {
   public:
    pmt::util::smrt::GenericAst const& _ast;
    GrammarData const& _grammar_data;
    LexerTables const& _lexer_tables;
  };

  // -$ Functions $-
  // --$ Other $--
 static auto build(Args args_) -> ParserTables;
};

}