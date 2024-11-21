#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/util/parse/generic_ast_printer.hpp"
#include "pmt/util/parse/grm_ast.hpp"
#include "pmt/util/parse/grm_ast_transformations.hpp"
#include "pmt/util/parse/grm_lexer.hpp"
#include "pmt/util/parse/grm_parser.hpp"
#include "pmt/util/parse/lexer_builder.hpp"

#include <fstream>
#include <iostream>

namespace pmt::parserbuilder {

ParserBuilder::ParserBuilder(std::string_view input_path_) {
  std::ifstream input_file(input_path_.data());
  _input = std::string(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>());
}

void ParserBuilder::build() {
  pmt::util::parse::GenericAstPrinter printer(&pmt::util::parse::GrmAst::to_string);
  pmt::util::parse::GrmLexer lexer(_input);
  auto ast = pmt::util::parse::GrmParser::parse(lexer);

  pmt::util::parse::LexerBuilder(*ast, {});

  pmt::util::parse::GrmAstTransformations::emit_grammar(std::cerr, *ast);
  // std::cout << "------------------------ AST ------------------------\n";
  // printer.print(*ast, std::cerr);
}

}  // namespace pmt::parserbuilder