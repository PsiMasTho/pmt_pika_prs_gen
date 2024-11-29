#pragma once

#include "pmt/util/parse/ast_position.hpp"
#include "pmt/util/parse/expression_frame_base.hpp"
#include "pmt/util/parse/fa_part.hpp"
#include "pmt/util/parse/generic_ast.hpp"

#include <vector>

namespace pmt::util::parse {

class ChoicesExpressionFrame : public ExpressionFrameBase {
 public:
  // - Lifetime -
  ChoicesExpressionFrame(AstPositionConst ast_position_);

  // - Virtual member functions -
  auto get_id() const -> GenericAst::IdType final;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  // - Private member functions -
  void process_stage_0(CallstackType& callstack_);
  void process_stage_1(CallstackType& callstack_, Captures& captures_);

  // - Data -
  std::vector<FaPart> _sub_parts;
  AstPositionConst _ast_position;
  size_t _stage;
};

}  // namespace pmt::util::parse