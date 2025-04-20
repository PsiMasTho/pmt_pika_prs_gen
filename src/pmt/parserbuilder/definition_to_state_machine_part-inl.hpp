// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/parserbuilder/definition_to_state_machine_part.hpp"
#endif
// clang-format on

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"
#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base_captures.hpp"
#include "pmt/parserbuilder/expression_to_state_machine_part_frame_factory.hpp"

namespace pmt::parserbuilder {

template <util::smct::IsStateTag TAG_>
auto DefinitionToStateMachinePart<TAG_>::convert(pmt::util::smct::StateMachine& dest_state_machine_, std::string const& name_, pmt::util::smrt::GenericAstPath const& definition_, std::vector<std::string> const& names_, std::vector<pmt::util::smrt::GenericAstPath> const& definitions_, pmt::util::smrt::GenericAst const& ast_root_) -> pmt::util::smct::StateMachinePart<TAG_> {
  ExpressionToStateMachinePartFrameBase<TAG_>::CallstackType callstack;
  ExpressionToStateMachinePartFrameBaseCaptures<TAG_> captures{._dest_state_machine = dest_state_machine_, ._names = names_, ._definitions = definitions_, ._ast = ast_root_};

  captures._name_stack_contents.insert(name_);
  captures._name_stack.push_back(name_);
  callstack.push(ExpressionToStateMachinePartFrameFactory::construct(ast_root_, terminal_definition_));

  while (!callstack.empty()) {
    ExpressionToStateMachinePartFrameBase::FrameHandle cur = callstack.top();
    callstack.pop();
    cur->process(callstack, captures);
  }

  return ret_part;
}

}  // namespace pmt::parserbuilder