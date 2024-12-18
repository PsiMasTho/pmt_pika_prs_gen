#pragma once

#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/util/parsert/generic_ast.hpp"

#include <string>

namespace pmt::parserbuilder {

class TerminalDefinitionToFaPart {
 public:
  static auto convert(pmt::util::parsect::Fa& dest_fa_, std::string const& terminal_name_, pmt::util::parsert::GenericAst::PositionConst terminal_definition_, std::unordered_map<std::string, pmt::util::parsert::GenericAst::PositionConst> const& terminal_definitions_) -> FaPart;
};

}  // namespace pmt::parserbuilder