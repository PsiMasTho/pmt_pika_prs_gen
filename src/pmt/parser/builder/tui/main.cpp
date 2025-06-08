#include "pmt/parser/grammar/grammar_data.hpp"

#include "pmt/parser/builder/tui/args.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/lexer_tables.hpp"
#include "pmt/parser/grammar/parser_tables.hpp"
#include "pmt/parser/grammar/lexer_tables.hpp"
#include "pmt/parser/builder/lexer_table_builder.hpp"
#include "pmt/parser/builder/lexer_table_writer.hpp"
#include "pmt/parser/builder/parser_table_builder.hpp"
#include "pmt/parser/builder/parser_table_writer.hpp"
#include "pmt/parser/builder/id_constants_writer.hpp"
#include "pmt/parser/grammar/nonterminal_inliner.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/rt/generic_parser.hpp"
#include "pmt/parser/rt/generic_lexer.hpp"
#include "pmt/parser/generic_ast_printer.hpp"
#include "pmt/parser/rt/generic_lexer.hpp"
#include "pmt/parser/rt/generic_parser.hpp"
#include "pmt/parser/builder/terminal_overlap_checker.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace pmt::parser::builder;
using namespace pmt::parser::grammar;
using namespace pmt::parser::rt;
using namespace pmt::parser;
using namespace pmt::parser::builder::tui;
using namespace pmt::util::sm;
using namespace pmt::util::sm::ct;
using namespace pmt::base;

namespace {

auto report_terminal_overlaps(GrammarData const& grammar_data_, std::unordered_set<IntervalSet<AcceptsIndexType>> const& overlapping_terminals_) {
 if (overlapping_terminals_.empty()) {
  return;
 }
 
 std::string msg = "Warning: Terminal overlaps found for: ";
 std::string delim_1;
 for (auto const& accepts : overlapping_terminals_) {
  std::string delim_2;
  msg += std::exchange(delim_1, ", ") + "{";
  
  accepts.for_each_key([&](AcceptsIndexType i_) {
   msg += std::exchange(delim_2, ", ") + grammar_data_.lookup_terminal_label_by_index(i_);
  });
  
  msg += "}";
 }

 std::cerr << msg << '\n';
}

}  // namespace

auto main(int argc, char const* const* argv) -> int try {
  Args const args(argc, argv);

  std::ifstream input_grammar_stream(args._input_grammar_file);
  std::string const input_grammar((std::istreambuf_iterator<char>(input_grammar_stream)), std::istreambuf_iterator<char>());

  pmt::parser::grammar::LexerTables const lexer_tables;
  GenericLexer lexer(input_grammar, lexer_tables);
  pmt::parser::grammar::ParserTables const parser_tables;
  GenericParser parser;
  GenericAst::UniqueHandle ast = parser.parse(lexer, parser_tables);

  GenericAstPrinter::Args ast_printer_args{
    ._id_to_string_fn = [&](GenericId::IdType id_) {return Ast::id_to_string(id_);},
    ._out = std::cout,
    ._ast = *ast,
  };

  std::cout << "---BEGIN Printing AST from generated tables:\n";
  GenericAstPrinter::print(ast_printer_args);
  std::cout << "---END Printing AST from generated tables\n";


  /* GrammarData grammar_data = GrammarData::construct_from_ast(*ast);
  NonterminalInliner::do_inline(NonterminalInliner::Args{._grammar_data = grammar_data, ._ast = *ast});

  std::cout << "Building lexer tables...\n";
  LexerTables const lexer_tables = LexerTableBuilder{}.build(*ast, grammar_data);

  std::cout << "Lexer tables built successfully\n";

  std::cout << "Writing lexer tables to header and source files...\n";
  std::ofstream lexer_header_file(args._output_lexer_header_file);
  std::ofstream lexer_source_file(args._output_lexer_source_file);

  std::ifstream lexer_header_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/lexer_tables-skel.hpp");
  std::ifstream lexer_source_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/lexer_tables-skel.cpp");

  LexerTableWriter::WriterArgs table_writer_args{
   ._os_header = lexer_header_file,
   ._os_source = lexer_source_file,
   ._is_header_skel = lexer_header_skel_file,
   ._is_source_skel = lexer_source_skel_file,
   ._tables = lexer_tables,
   ._namespace_name = args._namespace_name,
   ._class_name = args._lexer_class_name,
   ._header_include_path = std::filesystem::path(args._output_lexer_header_file).filename().string()
  };

  LexerTableWriter lexer_table_writer;
  lexer_table_writer.write(table_writer_args);
  std::cout << "Done writing lexer tables\n";

  std::cout << "Building parser tables...\n";
  ParserTables parser_tables = ParserTableBuilder::build(ParserTableBuilder::Args(*ast, grammar_data, lexer_tables));

  std::ofstream parser_header_file(args._output_parser_header_file);
  std::ofstream parser_source_file(args._output_parser_source_file);

  std::ifstream parser_header_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/parser_tables-skel.hpp");
  std::ifstream parser_source_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/parser_tables-skel.cpp");

  ParserTableWriter::WriterArgs parser_writer_args{
   ._os_header = parser_header_file,
   ._os_source = parser_source_file,
   ._is_header_skel = parser_header_skel_file,
   ._is_source_skel = parser_source_skel_file,
   ._tables = parser_tables,
   ._namespace_name = args._namespace_name,
   ._class_name = args._parser_class_name,
   ._header_include_path = std::filesystem::path(args._output_parser_header_file).filename().string()
  };
  ParserTableWriter parser_table_writer;
  parser_table_writer.write(parser_writer_args);
  std::cout << "Done writing parser tables\n";

  std::ofstream id_constants_file(args._output_id_constants_file);
  std::ifstream id_constants_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/id_constants-skel.hpp");
  IdConstantsWriter::WriterArgs id_constants_writer_args{
   ._os_id_constants = id_constants_file,
   ._is_id_constants_skel = id_constants_skel_file,
   ._lexer_tables = lexer_tables,
   ._parser_tables = parser_tables
  };

  IdConstantsWriter id_constants_writer;
  id_constants_writer.write(id_constants_writer_args);
  std::cout << "Done writing id constants\n";

  std::unordered_set<IntervalSet<AcceptsIndexType>> const overlapping_terminals = TerminalOverlapChecker::find_overlaps(
   TerminalOverlapChecker::Args{
    ._state_machine = parser_tables.get_parser_state_machine(),
    ._grammar_data = grammar_data,
    ._ast = *ast,
   }
  );

  report_terminal_overlaps(grammar_data, overlapping_terminals);

  GenericLexer test_lexer(test_input, lexer_tables);
  GenericParser test_parser;
  GenericAst::UniqueHandle test_ast = test_parser.parse(test_lexer, parser_tables);

  GenericAstPrinter::Args ast_printer_args{
    ._id_to_string_fn = [&](GenericId::IdType id_) {
     if (id_ < lexer_tables.get_min_id() + lexer_tables.get_id_count()) {
      return lexer_tables.id_to_string(id_);
     }
     if (id_ < parser_tables.get_min_id() + parser_tables.get_id_count()) {
      return parser_tables.id_to_string(id_);
     }
     return "unknown(" + std::to_string(id_) + ")";
    },
    ._out = std::cout,
    ._ast = *test_ast,
    ._indent_width = 2
  };

  GenericAstPrinter::print(ast_printer_args); */
} catch (std::exception const& e) {
  std::cerr << std::string(e.what()) << '\n';
  return 1;
} catch (...) {
  std::cerr << "Unhandled exception!\n";
  return 1;
}
