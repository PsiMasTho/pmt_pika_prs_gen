#pragma once

#include "pmt/util/parsect/state_machine.hpp"
#include "pmt/util/parsect/state_machine_part.hpp"
#include "pmt/util/parsert/generic_ast_path.hpp"

#include <string>
#include <unordered_set>
#include <vector>

namespace pmt::parserbuilder {

template <pmt::util::parsect::IsStateTag TAG_>
class ExpressionToStateMachinePartFrameBaseCaptures {
 public:
  pmt::util::parsect::StateMachinePart<TAG_> _ret_part;
  std::unordered_set<std::string> _name_stack_contents;
  std::vector<std::string> _name_stack;

  pmt::util::parsect::StateMachine<TAG_>& _dest_state_machine;
  std::vector<std::string> const& _names;
  std::vector<pmt::util::parsert::GenericAstPath> const& _definitions;
  pmt::util::parsert::GenericAst const& _ast;
};

}  // namespace pmt::parserbuilder