#include "pmt/parserbuilder/exe/args.hpp"

#include <stdexcept>
#include <set>

namespace pmt::parserbuilder::exe {

Args::Args(int argc_, char const* const* argv_) {
  for (int i = 1; i < argc_; ++i) {
    std::string const arg = argv_[i];
    if (arg == "-g") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -g");
      }
      _input_grammar_file = argv_[++i];
    } else if (arg == "-h") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -h");
      }
      _output_header_file = argv_[++i];
    } else if (arg == "-s") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -s");
      }
      _output_source_file = argv_[++i];
    } else if (arg == "-c") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -c");
      }
      _output_id_constants_file = argv_[++i];
     } else {
      throw std::runtime_error("Unknown argument: " + arg);
    }
  }

  if (_input_grammar_file.empty()) {
    throw std::runtime_error("Missing input file");
  }

  if (_output_header_file.empty()) {
    throw std::runtime_error("Missing output header file");
  }

  if (_output_source_file.empty()) {
    throw std::runtime_error("Missing output source file");
  }

  std::set<std::string> unique = {
    _input_grammar_file,
    _output_header_file,
    _output_source_file,
    _output_id_constants_file,
  };

  if (unique.size() != 4) {
    throw std::runtime_error("Duplicate file names provided");
  }
}

}  // namespace pmt::parserbuilder::exe