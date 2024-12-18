#include "pmt/parserbuilder/exe/args.hpp"

#include <stdexcept>

namespace pmt::parserbuilder::exe {

Args::Args(int argc_, char const* const* argv_) {
  // -g <input grammar>
  // -s <input sample>
  for (int i = 1; i < argc_; ++i) {
    std::string const arg = argv_[i];
    if (arg == "-g") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -g");
      }
      _input_grammar_file = argv_[++i];
    } else if (arg == "-s") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -s");
      }
      _input_sample_file = argv_[++i];
    } else {
      throw std::runtime_error("Unknown argument: " + arg);
    }
  }

  if (_input_grammar_file.empty()) {
    throw std::runtime_error("Missing input file");
  }

  if (_input_sample_file.empty()) {
    throw std::runtime_error("Missing input sample file");
  }
}

}  // namespace pmt::parserbuilder::exe