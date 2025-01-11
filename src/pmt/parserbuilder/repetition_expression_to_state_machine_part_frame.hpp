#pragma once

#include "pmt/parserbuilder/expression_to_state_machine_part_frame_base.hpp"
#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/parsect/fa.hpp"
#include "pmt/util/parsect/fa_part.hpp"

namespace pmt::parserbuilder {

class RepetitionExpressionToFaPartFrame : public ExpressionToStateMachinePartFrameBase<TAG_> {
 public:
  explicit RepetitionExpressionToFaPartFrame(pmt::util::parsert::GenericAstPath const& path_);
  void process(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_) final;

 private:
  void process_stage_0(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_1(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);
  void process_stage_2(CallstackType& callstack_, ExpressionToStateMachinePartFrameBaseCaptures& captures_);

  auto is_last() const -> bool;
  auto is_chunk_first() const -> bool;
  auto is_chunk_last() const -> bool;

  pmt::util::parsect::FaPart _choices;
  pmt::util::parsect::FaPart _chunk;
  GrmNumber::RepetitionRangeType _range;
  pmt::util::parsect::Fa::Transitions* _transitions_choices = nullptr;
  size_t _stage = 0;
  size_t _idx = 0;
  size_t _idx_max = 0;
  size_t _idx_chunk = 0;
};

}  // namespace pmt::parserbuilder