#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"
#include "pmt/parserbuilder/fa_part.hpp"

PMT_FW_DECL_NS_CLASS(pmt::util::parsect, Fa);

namespace pmt::parserbuilder {

class EpsilonExpressionToFaPartFrame : public ExpressionToFaPartFrameBase {
 public:
  using ExpressionToFaPartFrameBase::ExpressionToFaPartFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;

  static auto make_epsilon(pmt::util::parsect::Fa& fa_) -> FaPart;
};

}  // namespace pmt::parserbuilder