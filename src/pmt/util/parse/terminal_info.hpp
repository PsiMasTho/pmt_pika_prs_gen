#pragma once

#include "pmt/util/parse/generic_ast.hpp"

#include <string>

namespace pmt::util::parse {

class TerminalInfo {
 public:
  std::string _name;
  GenericAst::IdType _id;
};

}  // namespace pmt::util::parse