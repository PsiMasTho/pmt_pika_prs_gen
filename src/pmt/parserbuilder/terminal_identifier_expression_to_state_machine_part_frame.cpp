#include "pmt/parserbuilder/terminal_identifier_expression_to_state_machine_part_frame.hpp"

#include "pmt/base/algo.hpp"
#include "pmt/parserbuilder/expression_to_state_machine_part_frame_factory.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

#include <utility>

namespace pmt::parserbuilder {

void TerminalIdentifierExpressionToStateMachinePart::process(ExpressionToStateMachinePartFrameBase::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(captures_);
      break;
  }
}

void TerminalIdentifierExpressionToStateMachinePart::process_stage_0(ExpressionToStateMachinePartFrameBase::CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  _terminal_name = &_path.resolve(captures_._ast)->get_string();

  captures_._name_stack.push_back(*_terminal_name);
  if (!captures_._name_stack_contents.insert(*_terminal_name).second) {
    std::string msg = "Terminal '" + *_terminal_name + "' is recursive: ";
    std::string delim;
    for (std::string const& terminal : captures_._name_stack) {
      msg += std::exchange(delim, " -> ") + terminal;
    }
    throw std::runtime_error(msg);
  }

  ++_stage;
  callstack_.push(shared_from_this());

  if (std::optional<size_t> const index = pmt::base::binary_find_index(captures_._names.begin(), captures_._names.end(), *_terminal_name); index.has_value()) {
    _path = captures_._definitions[*index];
  } else {
    throw std::runtime_error("Terminal '" + *_terminal_name + "' not defined");
  }

  _state_nr_reference = captures_._dest_state_machine.create_new_state();
  _state_reference = captures_._dest_state_machine.get_state(_state_nr_reference);

  callstack_.push(ExpressionToStateMachinePartFrameFactory::construct(captures_._ast, _path));
}

void TerminalIdentifierExpressionToStateMachinePart::process_stage_1(ExpressionToStateMachinePartFrameBaseCaptures& captures_) {
  _state_reference->add_epsilon_transition(*captures_._ret_part.get_incoming_state_nr());
  captures_._ret_part.set_incoming_state_nr(_state_nr_reference);

  captures_._name_stack_contents.erase(*_terminal_name);
  captures_._name_stack.pop_back();
}

}  // namespace pmt::parserbuilder