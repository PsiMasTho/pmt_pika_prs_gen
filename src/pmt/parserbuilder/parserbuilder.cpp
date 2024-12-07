#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/parserbuilder/grm_lexer.hpp"
#include "pmt/parserbuilder/grm_parser.hpp"
#include "pmt/parserbuilder/lexer_builder.hpp"
#include "pmt/parserbuilder/table_writer.hpp"
#include "pmt/util/parse/generic_ast_printer.hpp"
#include "pmt/util/parse/generic_lexer.hpp"

#include <chrono>
#include <fstream>
#include <iostream>

namespace pmt::parserbuilder {

ParserBuilder::ParserBuilder(std::string_view input_path_, std::string_view input_sample_path_, std::set<std::string> const& terminals_)
 : _terminals(terminals_) {
  std::ifstream input_grammar(input_path_.data());
  _input_grammar = std::string(std::istreambuf_iterator<char>(input_grammar), std::istreambuf_iterator<char>());
  std::ifstream input_sample(input_sample_path_.data());
  _input_sample = std::string(std::istreambuf_iterator<char>(input_sample), std::istreambuf_iterator<char>());
}

void ParserBuilder::build() {
  GrmLexer lexer(_input_grammar);
  auto ast = GrmParser::parse(lexer);

  auto const start = std::chrono::high_resolution_clock::now();
  LexerBuilder lexer_builder(*ast, _terminals);
  pmt::util::parse::GenericLexerTables tables = lexer_builder.build();
  auto const end = std::chrono::high_resolution_clock::now();
  std::cout << "Lexer build time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
  std::cout << "State count: " << tables._transitions.size() << std::endl;

  pmt::util::parse::GenericLexer generic_lexer(_input_sample, tables);

  pmt::base::DynamicBitset const accepts_all(tables._terminal_ids.size(), true);

  auto const to_string = [&tables](pmt::util::parse::GenericAst::IdType id_) -> std::string {
    if (id_ < tables._id_names.size()) {
      return tables._id_names[id_];
    }

    return "Unknown token: " + std::to_string(id_);
  };
  pmt::util::parse::GenericAstPrinter printer(to_string);

  while (true) {
    pmt::util::parse::GenericAst::UniqueHandle token = generic_lexer.next_token(accepts_all);
    printer.print(*token, std::cerr);
    if (token->get_id() == pmt::util::parse::GenericAst::IdEoi) {
      break;
    }
  }

  std::ofstream os_header("lexer_tables.hpp");
  std::ofstream os_source("lexer_tables.cpp");
  TableWriter table_writer(os_header, os_source, "TestLexerTables", tables);
  table_writer.write();
}

}  // namespace pmt::parserbuilder