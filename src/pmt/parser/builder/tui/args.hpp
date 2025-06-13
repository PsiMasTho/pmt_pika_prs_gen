#pragma once

#include <fstream>
#include <optional>
#include <string>

namespace pmt::parser::builder::tui {

class Args {
 public:
  Args(int argc_, char const* const* argv_);

  std::ifstream _input_grammar_file;
  std::optional<std::ifstream> _input_test_file;

  std::string _lexer_header_include_filename;
  std::ofstream _output_lexer_header_file;
  std::ofstream _output_lexer_source_file;

  std::ifstream _lexer_header_skel_file;
  std::ifstream _lexer_source_skel_file;

  std::string _parser_header_include_filename;
  std::ofstream _output_parser_header_file;
  std::ofstream _output_parser_source_file;

  std::ifstream _parser_header_skel_file;
  std::ifstream _parser_source_skel_file;

  std::ofstream _output_id_constants_file;

  std::ifstream _id_constants_skel_file;

  std::string _lexer_class_name;
  std::string _parser_class_name;
  std::string _namespace_name;
};

}  // namespace pmt::parser::builder::tui