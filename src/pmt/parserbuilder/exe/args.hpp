#pragma once

#include <set>
#include <string>

namespace pmt::parserbuilder::exe {

class Args {
 public:
  Args(int argc_, char const* const* argv_);

  std::set<std::string> _terminals;
  std::string _input_file;
};

}  // namespace pmt::parserbuilder::exe