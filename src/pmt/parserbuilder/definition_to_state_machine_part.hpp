#pragma once

#include "pmt/util/parsect/state_machine.hpp"
#include "pmt/util/parsect/state_machine_part.hpp"
#include "pmt/util/parsert/generic_ast.hpp"
#include "pmt/util/parsert/generic_ast_path.hpp"

#include <string>

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
class DefinitionToStateMachinePart {
 public:
  static auto convert(pmt::util::parsect::StateMachine& dest_state_machine_, std::string const& name_, pmt::util::parsert::GenericAstPath const& definition_, std::vector<std::string> const& names_, std::vector<pmt::util::parsert::GenericAstPath> const& definitions_, pmt::util::parsert::GenericAst const& ast_root_) -> pmt::util::parsect::StateMachinePart<TAG_>;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/definition_to_state_machine_part-inl.hpp"