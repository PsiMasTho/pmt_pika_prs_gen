#include "pmt/parserbuilder/parserbuilder.hpp"

#include "pmt/parserbuilder/exe/args.hpp"

#include <iostream>

auto main(int argc, char const* const* argv) -> int try {
  pmt::parserbuilder::exe::Args const args(argc, argv);

  pmt::parserbuilder::ParserBuilder builder(args._input_grammar_file, args._input_sample_file, args._terminals);
  builder.build();

  return 0;
} catch (std::exception const& e) {
  std::cerr << std::string(e.what()) << '\n';
  return 1;
} catch (...) {
  std::cerr << "Unhandled exception!\n";
  return 1;
}
