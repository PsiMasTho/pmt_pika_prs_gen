#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/parsert/generic_ast.hpp"

#include <memory>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, FaPart);
PMT_FW_DECL_NS_CLASS(pmt::util::parsect, Fa);

namespace pmt::parserbuilder {

class ExpressionToFaPartFrameBase : public std::enable_shared_from_this<ExpressionToFaPartFrameBase> {
 public:
  using FrameHandle = std::shared_ptr<ExpressionToFaPartFrameBase>;
  using CallstackType = std::stack<FrameHandle>;

  class Captures;

  explicit ExpressionToFaPartFrameBase(pmt::util::parsert::GenericAst::AstPositionConst ast_position_);
  virtual ~ExpressionToFaPartFrameBase() = default;

  virtual void process(CallstackType& callstack_, Captures& captures_) = 0;

 protected:
  pmt::util::parsert::GenericAst::AstPositionConst _ast_position;
};

class ExpressionToFaPartFrameBase::Captures {
 public:
  FaPart& _ret_part;
  pmt::util::parsect::Fa& _result;
  std::vector<std::string> const& _terminal_names;
  std::vector<pmt::util::parsert::GenericAst::AstPositionConst> const& _terminal_definitions;
  std::unordered_set<std::string>& _terminal_stack_contents;
  std::vector<std::string>& _terminal_stack;
};
}  // namespace pmt::parserbuilder