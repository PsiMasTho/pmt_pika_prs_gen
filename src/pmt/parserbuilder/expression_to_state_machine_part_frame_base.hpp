#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base_captures.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

#include <memory>
#include <stack>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst);

namespace pmt::parserbuilder {

template <pmt::util::smct::IsStateTag TAG_>
class ExpressionToStateMachinePartFrameBase : public std::enable_shared_from_this<ExpressionToStateMachinePartFrameBase<TAG_>> {
 public:
  using FrameHandle = std::shared_ptr<ExpressionToStateMachinePartFrameBase>;
  using CallstackType = std::stack<FrameHandle>;

  explicit ExpressionToStateMachinePartFrameBase(pmt::util::smrt::GenericAstPath const& path_);
  virtual ~ExpressionToStateMachinePartFrameBase() = default;

  virtual void process(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures<TAG_>& captures_) = 0;

 protected:
  pmt::util::smrt::GenericAstPath _path;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base-inl.hpp"