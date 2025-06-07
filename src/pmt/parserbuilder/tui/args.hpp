#pragma once

#include <string>

namespace pmt::parserbuilder::tui {

class Args {
 public:
  Args(int argc_, char const* const* argv_);

  std::string _input_grammar_file;
  std::string _input_test_file;

  std::string _output_lexer_header_file;
  std::string _output_lexer_source_file;
  std::string _output_lexer_id_constants_file;

  std::string _output_parser_header_file;
  std::string _output_parser_source_file;
  std::string _output_parser_id_constants_file;

  bool _print_ast_from_generated_tables = false;
};

}  // namespace pmt::parserbuilder::tui