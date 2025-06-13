#include "pmt/parser/grammar/ast.hpp"

#include "pmt/parser/grammar/lexer_tables.hpp"
#include "pmt/parser/grammar/parser_tables.hpp"

namespace pmt::parser::grammar {
auto Ast::id_to_string(GenericId::IdType id_) -> std::string {
  static LexerTables const lexer_tables;
  if (id_ < lexer_tables.get_min_id() + lexer_tables.get_id_count()) {
    return lexer_tables.id_to_string(id_);
  }

  static ParserTables const parser_tables;
  if (id_ < parser_tables.get_min_id() + parser_tables.get_id_count()) {
    return parser_tables.id_to_string(id_);
  }

  switch (id_) {
    case NtTerminalHidden:
      return "NtTerminalHidden";
    default:
      throw std::runtime_error("Invalid id");
  }
}
}  // namespace pmt::parser::grammar