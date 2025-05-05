#include "pmt/parserbuilder/grammar_data.hpp"

#include "pmt/parserbuilder/tui/args.hpp"
#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/parserbuilder/grm_parser.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/parserbuilder/lexer_table_builder.hpp"
#include "pmt/parserbuilder/lexer_table_writer.hpp"
#include "pmt/util/smct/graph_writer.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace pmt::parserbuilder;
using namespace pmt::parserbuilder::tui;
using namespace pmt::util::smrt;
using namespace pmt::util::smct;

namespace {
const char* const DOT_FILE_PREFIX = "graph_";
size_t const DOT_FILE_MAX_STATES = 750;

void write_dot(LexerTables const& lexer_tables_) {
  static size_t dot_file_count = 0;

  if (lexer_tables_.get_state_count() > DOT_FILE_MAX_STATES) {
    std::cerr << "Skipping dot file write, too many states\n";
    return;
  }

  std::ofstream graph_file(DOT_FILE_PREFIX + std::to_string(dot_file_count++) + ".dot");
  std::ifstream skel_file("/home/pmt/repos/pmt/skel/pmt/util/smct/state_machine-skel.dot");

  GraphWriter::WriterArgs writer_args{
    ._os_graph = graph_file,
    ._is_graph_skel = skel_file,
    ._state_machine = lexer_tables_
  };

  GraphWriter::StyleArgs style_args;
  style_args._accepts_to_label_fn = [&](size_t accepts_) -> std::string {return lexer_tables_.get_terminal_label(accepts_);};
  
  GraphWriter().write_dot(writer_args, style_args);
}

}  // namespace

auto main(int argc, char const* const* argv) -> int try {
  Args const args(argc, argv);

  std::ifstream input_grammar_stream(args._input_grammar_file);
  std::string const input_grammar((std::istreambuf_iterator<char>(input_grammar_stream)), std::istreambuf_iterator<char>());

  GrmLexer lexer(input_grammar);
  GenericAst::UniqueHandle ast = GrmParser::parse(lexer);
  GrammarData grammar_data = GrammarData::construct_from_ast(*ast);

  LexerTables lexer_tables = LexerTableBuilder{}.build(*ast, grammar_data);

  std::cout << "Lexer tables built successfully\n";
  write_dot(lexer_tables);
  std::cout << "Dot file written\n";

  std::cout << "Writing lexer tables to header and source files...\n";
  std::ofstream header_file(args._output_header_file);
  std::ofstream source_file(args._output_source_file);
  std::ofstream id_constants_file(args._output_id_constants_file);

  std::ifstream header_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/lexer_tables-skel.hpp");
  std::ifstream source_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/lexer_tables-skel.cpp");
  std::ifstream id_constants_skel_file("/home/pmt/repos/pmt/skel/pmt/parserbuilder/lexer_id_constants-skel.hpp");

  LexerTableWriter::WriterArgs table_writer_args{
   ._os_header = header_file,
   ._os_source = source_file,
   ._os_id_constants = id_constants_file,
   ._is_header_skel = header_skel_file,
   ._is_source_skel = source_skel_file,
   ._is_id_constants_skel = id_constants_skel_file,
   ._tables = lexer_tables,
   ._namespace_name = "pmt::parserbuilder",
   ._class_name = "LexerClass",
   ._header_include_path = std::filesystem::path(args._output_header_file).filename().string()
  };

  LexerTableWriter table_writer;
  table_writer.write(table_writer_args);
  std::cout << "Done writing lexer tables\n";

} catch (std::exception const& e) {
  std::cerr << std::string(e.what()) << '\n';
  return 1;
} catch (...) {
  std::cerr << "Unhandled exception!\n";
  return 1;
}
