#include "pmt/parserbuilder/parserbuilder.hpp"

#include <iostream>

auto main(int argc, char const* const* argv) -> int {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input file>\n";
    return 1;
  }

  pmt::parserbuilder::ParserBuilder builder(argv[1]);

  try {
    builder.build();
  } catch (std::exception const& e) {
    std::cerr << "Unhandled exception: " + std::string(e.what()) << '\n';
  } catch (...) {
    std::cerr << "Unhandled exception!\n";
  }
}