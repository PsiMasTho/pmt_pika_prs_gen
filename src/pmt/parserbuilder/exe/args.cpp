#include "pmt/parserbuilder/exe/args.hpp"

#include <stdexcept>

namespace pmt::parserbuilder::exe {

Args::Args(int argc_, char const* const* argv_) {
  // -i <input file>
  // -t <terminals>...
  for (int i = 1; i < argc_; ++i) {
    std::string const arg = argv_[i];
    if (arg == "-i") {
      if (i + 1 >= argc_) {
        throw std::runtime_error("Missing argument for -i");
      }
      _input_file = argv_[++i];
    } else if (arg == "-t") {
      while (++i < argc_) {
        std::string const terminal = argv_[i];
        if (terminal.empty() || terminal[0] == '-') {
          --i;
          break;
        }
        _terminals.insert("$" + terminal);
      }
    } else {
      throw std::runtime_error("Unknown argument: " + arg);
    }
  }

  if (_input_file.empty()) {
    throw std::runtime_error("Missing input file");
  }

  if (_terminals.empty()) {
    throw std::runtime_error("No terminals specified");
  }
}

}  // namespace pmt::parserbuilder::exe