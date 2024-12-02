#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/util/parse/generic_ast_printer.hpp"
#include "pmt/util/parse/graph_writer.hpp"
#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/grm_ast_transformations.hpp"
#include "pmt/util/parse/grm_lexer.hpp"
#include "pmt/util/parse/grm_parser.hpp"
#include "pmt/util/parse/lexer_builder.hpp"

#include <chrono>
#include <fstream>
#include <iostream>

namespace pmt::parserbuilder {

ParserBuilder::ParserBuilder(std::string_view input_path_, std::set<std::string> const& terminals_)
 : _terminals(terminals_) {
  std::ifstream input_file(input_path_.data());
  _input = std::string(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>());
}

void ParserBuilder::build() {
  pmt::util::parse::GenericAstPrinter printer(&pmt::util::parse::GrmAst::to_string);
  pmt::util::parse::GrmLexer lexer(_input);
  auto ast = pmt::util::parse::GrmParser::parse(lexer);

  auto const start = std::chrono::high_resolution_clock::now();
  pmt::util::parse::LexerBuilder lexer_builder(*ast, _terminals);
  pmt::util::parse::Fa fa = lexer_builder.build();
  auto const end = std::chrono::high_resolution_clock::now();
  std::cout << "Lexer build time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
  std::cout << "State count: " << fa._states.size() << std::endl;

  if (fa._states.size() < 500) {
    std::ofstream dot_file("lexer.dot");
    pmt::util::parse::GraphWriter::write_dot(dot_file, fa);
  } else {
    std::cout << "State count too high to generate dot file\n";
  }

  // std::cout << "------------------------ AST ------------------------\n";
  //  pmt::util::parse::GrmAstTransformations::emit_grammar(std::cerr, *ast);

  printer.print(*ast, std::cerr);
}

}  // namespace pmt::parserbuilder