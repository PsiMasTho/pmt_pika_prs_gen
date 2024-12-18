#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/parserbuilder/grm_parser.hpp"
#include "pmt/parserbuilder/lexer_builder.hpp"
#include "pmt/parserbuilder/table_writer.hpp"
#include "pmt/util/parsert/generic_ast_printer.hpp"
#include "pmt/util/parsert/generic_lexer.hpp"

#include <fstream>
#include <iostream>

namespace pmt::parserbuilder {
using namespace pmt::util::parsert;

ParserBuilder::ParserBuilder(std::string_view input_path_, std::string_view input_sample_path_) {
  std::ifstream input_grammar(input_path_.data());
  _input_grammar = std::string(std::istreambuf_iterator<char>(input_grammar), std::istreambuf_iterator<char>());
  std::ifstream input_sample(input_sample_path_.data());
  _input_sample = std::string(std::istreambuf_iterator<char>(input_sample), std::istreambuf_iterator<char>());
}

void ParserBuilder::build() {
  GrmLexer lexer(_input_grammar);
  auto ast = GrmParser::parse(lexer);

  pmt::util::parsert::GenericAstPrinter printer1(GrmAst::id_to_string);
  printer1.print(*ast, std::cerr);
}

}  // namespace pmt::parserbuilder