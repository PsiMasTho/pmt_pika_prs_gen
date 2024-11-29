#pragma once

#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/expression_frame_base.hpp"
#include "pmt/util/parse/fa_part.hpp"
#include "pmt/util/parse/generic_ast.hpp"

namespace pmt::util::parse {

class SequenceExpressionFrame : public ExpressionFrameBase {
 public:
  // - Lifetime -
  SequenceExpressionFrame(AstPositionConst ast_position_);

  // - Virtual member functions -
  auto get_id() const -> GenericAst::IdType final;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  // - Private member functions -
  void process_stage_0(CallstackType& callstack_);
  void process_stage_1(CallstackType& callstack_, Captures& captures_);

  // - Data -
  FaPart _sub_part;
  AstPositionConst _ast_position;
  size_t _stage;
  size_t _idx;
};

}  // namespace pmt::util::parse