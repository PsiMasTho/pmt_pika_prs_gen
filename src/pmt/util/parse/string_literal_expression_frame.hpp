#pragma once

#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/expression_frame_base.hpp"
#include "pmt/util/parse/generic_ast.hpp"

namespace pmt::util::parse {

class StringLiteralExpressionFrame : public ExpressionFrameBase {
 public:
  // - Lifetime -
  StringLiteralExpressionFrame(AstPositionConst ast_position_);

  // - Virtual member functions -
  auto get_id() const -> GenericAst::IdType final;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  // - Data -
  AstPositionConst _ast_position;
};

}  // namespace pmt::util::parse