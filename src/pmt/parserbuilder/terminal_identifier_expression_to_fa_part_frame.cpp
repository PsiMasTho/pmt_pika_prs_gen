
#include "pmt/parserbuilder/terminal_identifier_expression_to_fa_part_frame.hpp"

#include "pmt/base/algo.hpp"
#include "pmt/parserbuilder/expression_to_fa_part_frame_factory.hpp"
#include "pmt/parserbuilder/fa_part.hpp"

#include <utility>

namespace pmt::parserbuilder {

void TerminalIdentifierExpressionToFaPart::process(CallstackType& callstack_, Captures& captures_) {
  switch (_stage) {
    case 0:
      process_stage_0(callstack_, captures_);
      break;
    case 1:
      process_stage_1(captures_);
      break;
  }
}

void TerminalIdentifierExpressionToFaPart::process_stage_0(CallstackType& callstack_, Captures& captures_) {
  _terminal_name = &_ast_position.first->get_child_at(_ast_position.second)->get_string();

  captures_._terminal_stack.push_back(*_terminal_name);
  if (!captures_._terminal_stack_contents.insert(*_terminal_name).second) {
    std::string msg = "Terminal '" + *_terminal_name + "' is recursive: ";
    std::string delim;
    for (std::string const& terminal : captures_._terminal_stack) {
      msg += std::exchange(delim, " -> ") + terminal;
    }
    throw std::runtime_error(msg);
  }

  ++_stage;
  callstack_.push(shared_from_this());

  if (std::optional<size_t> const index = pmt::base::binary_find_index(captures_._terminal_names.begin(), captures_._terminal_names.end(), *_terminal_name); index.has_value()) {
    _ast_position = captures_._terminal_definitions[*index];
  } else {
    throw std::runtime_error("Terminal '" + *_terminal_name + "' not defined");
  }

  _state_nr_reference = captures_._result.get_unused_state_nr();
  _transitions_reference = &captures_._result._states[_state_nr_reference]._transitions;

  callstack_.push(ExpressionToFaPartFrameFactory::construct(_ast_position));
}

void TerminalIdentifierExpressionToFaPart::process_stage_1(Captures& captures_) {
  _transitions_reference->_epsilon_transitions.insert(*captures_._ret_part.get_incoming_state_nr());
  captures_._ret_part.set_incoming_state_nr(_state_nr_reference);

  captures_._terminal_stack_contents.erase(*_terminal_name);
  captures_._terminal_stack.pop_back();
}

}  // namespace pmt::parserbuilder