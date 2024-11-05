#include "pmt/parserbuilder/parserbuilder.hpp"

#include <iostream>

auto main(int argc, char const* const* argv) -> int {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
    return 1;
  }

  pmt::parserbuilder::ParserBuilder builder(argv[1]);
  builder.build();
}