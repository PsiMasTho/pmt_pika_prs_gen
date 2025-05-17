#include "pmt/parserbuilder/tui/args.hpp"

#include <stdexcept>
#include <set>

namespace pmt::parserbuilder::tui {

Args::Args(int argc_, char const* const* argv_) {
  for (int i = 1; i < argc_; ++i) {
    std::string const arg = argv_[i];
    if (arg == "-g") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -g");
      }
      _input_grammar_file = argv_[++i];
    } else if (arg == "-t") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -t");
      }
      _input_test_file = argv_[++i];
    } else if (arg == "-hl") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -hl");
      }
      _output_lexer_header_file = argv_[++i];
    } else if (arg == "-sl") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -sl");
      }
      _output_lexer_source_file = argv_[++i];
    } else if (arg == "-cl") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -cl");
      }
      _output_lexer_id_constants_file = argv_[++i];
    } else if (arg == "-hp") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -hp");
      }
      _output_parser_header_file = argv_[++i];
    } else if (arg == "-sp") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -sp");
      }
      _output_parser_source_file = argv_[++i];
    } else if (arg == "-cp") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -cp");
      }
      _output_parser_id_constants_file = argv_[++i];
    } else {
      throw std::runtime_error("Unknown argument: " + arg);
    }
  }

  if (_input_grammar_file.empty()) {
    throw std::runtime_error("Missing input grammar file");
  }

  if (_input_test_file.empty()) {
    throw std::runtime_error("Missing input test file");
  }

  if (_output_lexer_header_file.empty()) {
    throw std::runtime_error("Missing output lexer header file");
  }

  if (_output_lexer_source_file.empty()) {
    throw std::runtime_error("Missing output lexer source file");
  }

  if (_output_lexer_id_constants_file.empty()) {
    throw std::runtime_error("Missing output lexer id constants file");
  }

  if (_output_parser_header_file.empty()) {
    throw std::runtime_error("Missing output parser header file");
  }

  if (_output_parser_source_file.empty()) {
    throw std::runtime_error("Missing output parser source file");
  }

  if (_output_parser_id_constants_file.empty()) {
    throw std::runtime_error("Missing output parser id constants file");
  }

  std::set<std::string> unique = {
    _input_grammar_file,
    _input_test_file,
    _output_lexer_header_file,
    _output_lexer_source_file,
    _output_lexer_id_constants_file,
    _output_parser_header_file,
    _output_parser_source_file,
    _output_parser_id_constants_file
  };

  if (unique.size() != 8) {
    throw std::runtime_error("Duplicate file names provided");
  }
}

}  // namespace pmt::parserbuilder::exe