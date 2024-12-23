#pragma once

#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/util/parsert/generic_ast.hpp"

#include <string>

namespace pmt::parserbuilder {

class TerminalDefinitionToFaPart {
 public:
  static auto convert(pmt::util::parsect::Fa& dest_fa_, std::string const& terminal_name_, pmt::util::parsert::GenericAst::AstPositionConst terminal_definition_, std::vector<std::string> const& terminal_names_, std::vector<pmt::util::parsert::GenericAst::AstPositionConst> const& terminal_definitions_) -> FaPart;
};

}  // namespace pmt::parserbuilder