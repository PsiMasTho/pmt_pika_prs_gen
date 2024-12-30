#pragma once

#include "pmt/parserbuilder/expression_to_fa_part_frame_base.hpp"
#include "pmt/parserbuilder/fa_part.hpp"
#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/parsect/fa.hpp"

namespace pmt::parserbuilder {

class RepetitionExpressionToFaPartFrame : public ExpressionToFaPartFrameBase {
 public:
  explicit RepetitionExpressionToFaPartFrame(pmt::util::parsert::GenericAstPath const& path_);
  void process(CallstackType& callstack_, Captures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, Captures& captures_);
  void process_stage_1(CallstackType& callstack_, Captures& captures_);
  void process_stage_2(CallstackType& callstack_, Captures& captures_);

  auto is_last() const -> bool;
  auto is_chunk_first() const -> bool;
  auto is_chunk_last() const -> bool;

  FaPart _choices;
  FaPart _chunk;
  GrmNumber::RepetitionRangeType _range;
  pmt::util::parsect::Fa::Transitions* _transitions_choices = nullptr;
  size_t _stage = 0;
  size_t _idx = 0;
  size_t _idx_max = 0;
  size_t _idx_chunk = 0;
};

}  // namespace pmt::parserbuilder