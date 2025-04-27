#include "pmt/parserbuilder/grammar_data.hpp"

#include "pmt/parserbuilder/exe/args.hpp"
#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/parserbuilder/grm_parser.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/util/smct/graph_writer.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

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

  return 0;
} catch (std::exception const& e) {
  std::cerr << std::string(e.what()) << '\n';
  return 1;
} catch (...) {
  std::cerr << "Unhandled exception!\n";
  return 1;
}
