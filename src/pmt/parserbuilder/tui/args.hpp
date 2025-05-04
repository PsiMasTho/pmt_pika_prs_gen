#pragma once

#include <string>

namespace pmt::parserbuilder::tui {

class Args {
 public:
  Args(int argc_, char const* const* argv_);

  std::string _input_grammar_file;
  std::string _output_header_file;
  std::string _output_source_file;
  std::string _output_id_constants_file;
};

}  // namespace pmt::parserbuilder::tui