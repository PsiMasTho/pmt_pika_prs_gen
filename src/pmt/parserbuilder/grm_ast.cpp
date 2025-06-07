#include "pmt/parserbuilder/grm_ast.hpp"

#include "pmt/parserbuilder/grm_lexer_tables.hpp"
#include "pmt/parserbuilder/grm_parser_tables.hpp"

namespace pmt::parserbuilder {
auto GrmAst::id_to_string(pmt::util::smrt::GenericId::IdType id_) -> std::string {
 static GrmLexerTables const lexer_tables;
 if (id_ < lexer_tables.get_min_id() + lexer_tables.get_id_count()) {
  return lexer_tables.id_to_string(id_);
 }
 
 static GrmParserTables const parser_tables;
 if (id_ < parser_tables.get_min_id() + parser_tables.get_id_count()) {
  return parser_tables.id_to_string(id_);
 }
 
 // If the id is not found in either lexer or parser tables, throw an error
 throw std::runtime_error("Invalid id");
}
}  // namespace pmt::parserbuilder