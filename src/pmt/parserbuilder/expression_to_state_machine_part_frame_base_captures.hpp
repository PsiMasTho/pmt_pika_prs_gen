#pragma once

#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

#include <string>
#include <unordered_set>
#include <vector>

namespace pmt::parserbuilder {

template <pmt::util::smct::IsStateTag TAG_>
class ExpressionToStateMachinePartFrameBaseCaptures {
 public:
  pmt::util::smct::StateMachinePart<TAG_> _ret_part;
  std::unordered_set<std::string> _name_stack_contents;
  std::vector<std::string> _name_stack;

  pmt::util::smct::StateMachine& _dest_state_machine;
  std::vector<std::string> const& _names;
  std::vector<pmt::util::smrt::GenericAstPath> const& _definitions;
  pmt::util::smrt::GenericAst const& _ast;
};

}  // namespace pmt::parserbuilder