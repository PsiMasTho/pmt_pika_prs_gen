#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/builder/parser_tables.hpp"

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst);
PMT_FW_DECL_NS_CLASS(pmt::parser::grammar, GrammarData);
PMT_FW_DECL_NS_CLASS(pmt::parser::builder, LexerTables);

namespace pmt::parser::builder {

class ParserTableBuilder {
 public:
  // -$ Types / Constants $-
  class Args {
   public:
    GenericAst const& _ast;
    pmt::parser::grammar::GrammarData const& _grammar_data;
    LexerTables const& _lexer_tables;
    bool _write_dotfiles : 1 = true;
  };

  // -$ Functions $-
  // --$ Other $--
  static auto build(Args args_) -> ParserTables;
};

}  // namespace pmt::parser::builder