#include "pmt/parserbuilder/terminal_definition_to_fa_part.hpp"

#include "pmt/parserbuilder/expression_to_fa_part_frame_factory.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

auto TerminalDefinitionToFaPart::convert(Fa& dest_fa_, std::string const& terminal_name_, GenericAst::PositionConst terminal_definition_, std::unordered_map<std::string, GenericAst::PositionConst> const& terminal_definitions_) -> FaPart {
  FaPart ret_part;
  ExpressionToFaPartFrameBase::CallstackType callstack;
  std::unordered_set<std::string> terminal_stack_contents;
  std::vector<std::string> terminal_stack;
  ExpressionToFaPartFrameBase::Captures captures{ret_part, dest_fa_, terminal_definitions_, terminal_stack_contents, terminal_stack};

  terminal_stack_contents.insert(terminal_name_);
  terminal_stack.push_back(terminal_name_);
  callstack.push(ExpressionToFaPartFrameFactory::construct(terminal_definition_));

  while (!callstack.empty()) {
    ExpressionToFaPartFrameBase::FrameHandle cur = callstack.top();
    callstack.pop();
    cur->process(callstack, captures);
  }
}

}  // namespace pmt::parserbuilder