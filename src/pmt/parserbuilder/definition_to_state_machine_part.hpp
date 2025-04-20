#pragma once

#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

#include <string>

namespace pmt::parserbuilder {

template <pmt::util::smct::IsStateTag TAG_>
class DefinitionToStateMachinePart {
 public:
  static auto convert(pmt::util::smct::StateMachine& dest_state_machine_, std::string const& name_, pmt::util::smrt::GenericAstPath const& definition_, std::vector<std::string> const& names_, std::vector<pmt::util::smrt::GenericAstPath> const& definitions_, pmt::util::smrt::GenericAst const& ast_root_) -> pmt::util::smct::StateMachinePart<TAG_>;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/definition_to_state_machine_part-inl.hpp"