#pragma once

#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"
#include "pmt/util/parsect/fa.hpp"

#include <limits>

namespace pmt::parserbuilder {

class TerminalIdentifierExpressionToFaPart : public ExpressionToFaPartFrameBase {
 public:
  using ExpressionToFaPartFrameBase::ExpressionToFaPartFrameBase;
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, Captures& captures_);
  void process_stage_1(Captures& captures_);

  std::string const* _terminal_name = nullptr;
  pmt::util::parsect::Fa::Transitions* _transitions_reference = nullptr;
  pmt::util::parsect::Fa::StateNrType _state_nr_reference = std::numeric_limits<pmt::util::parsect::Fa::Fa::StateNrType>::max();
  size_t _stage = 0;
};

}  // namespace pmt::parserbuilder