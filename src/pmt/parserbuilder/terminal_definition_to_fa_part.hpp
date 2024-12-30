#pragma once

#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/util/parsert/generic_ast.hpp"
#include "pmt/util/parsert/generic_ast_path.hpp"

#include <string>

namespace pmt::parserbuilder {

class TerminalDefinitionToFaPart {
 public:
  static auto convert(pmt::util::parsect::Fa& dest_fa_, std::string const& terminal_name_, pmt::util::parsert::GenericAstPath const& terminal_definition_, std::vector<std::string> const& terminal_names_, std::vector<pmt::util::parsert::GenericAstPath> const& terminal_definitions_, pmt::util::parsert::GenericAst const& ast_root_) -> FaPart;
};

}  // namespace pmt::parserbuilder