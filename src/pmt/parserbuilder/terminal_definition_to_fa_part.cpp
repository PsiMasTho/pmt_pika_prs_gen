#include "pmt/parserbuilder/terminal_definition_to_fa_part.hpp"

#include "pmt/parserbuilder/expression_to_fa_part_frame_factory.hpp"

namespace pmt::parserbuilder {
using namespace pmt::util::parsect;
using namespace pmt::util::parsert;

auto TerminalDefinitionToFaPart::convert(pmt::util::parsect::Fa& dest_fa_, std::string const& terminal_name_, pmt::util::parsert::GenericAst::AstPositionConst terminal_definition_, std::vector<std::string> const& terminal_names_, std::vector<pmt::util::parsert::GenericAst::AstPositionConst> const& terminal_definitions_) -> FaPart {
  FaPart ret_part;
  ExpressionToFaPartFrameBase::CallstackType callstack;
  std::unordered_set<std::string> terminal_stack_contents;
  std::vector<std::string> terminal_stack;
  ExpressionToFaPartFrameBase::Captures captures{ret_part, dest_fa_, terminal_names_, terminal_definitions_, terminal_stack_contents, terminal_stack};

  terminal_stack_contents.insert(terminal_name_);
  terminal_stack.push_back(terminal_name_);
  callstack.push(ExpressionToFaPartFrameFactory::construct(terminal_definition_));

  while (!callstack.empty()) {
    ExpressionToFaPartFrameBase::FrameHandle cur = callstack.top();
    callstack.pop();
    cur->process(callstack, captures);
  }

  return ret_part;
}

}  // namespace pmt::parserbuilder