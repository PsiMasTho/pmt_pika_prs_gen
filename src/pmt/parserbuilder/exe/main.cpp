#include "pmt/parserbuilder/grammar_data.hpp"

#include "pmt/parserbuilder/exe/args.hpp"
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
using namespace pmt::parserbuilder::exe;
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

  std::ofstream file(DOT_FILE_PREFIX + std::to_string(dot_file_count++) + ".dot");
  GraphWriter::write_dot(file, lexer_tables_, [&lexer_tables_](size_t accept_idx_) { return lexer_tables_.get_accept_string(accept_idx_); });
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

  LexerTableWriter table_writer(header_file, source_file, id_constants_file, lexer_tables);

  LexerTableWriter::Arguments lexer_table_writer_args;
  lexer_table_writer_args._namespace_name = "pmt::parserbuilder";
  lexer_table_writer_args._class_name = "LexerClass";
  lexer_table_writer_args._header_include_path = std::filesystem::path(args._output_header_file).filename().string();
  lexer_table_writer_args._indent_increment = 1;
  table_writer.write(lexer_table_writer_args);
  std::cout << "Done writing lexer tables\n";

} catch (std::exception const& e) {
  std::cerr << std::string(e.what()) << '\n';
  return 1;
} catch (...) {
  std::cerr << "Unhandled exception!\n";
  return 1;
}
