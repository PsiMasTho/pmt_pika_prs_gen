#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/smct/state.hpp"
#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"
#include "pmt/util/smrt/generic_id.hpp"

#include <functional>
#include <string>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::parserbuilder {

class TerminalStateMachinePartBuilder {
 public:
  // -$ Types / Constants $-
  using TerminalLabelLookupFn = std::function<std::string(size_t)>;
  using TerminalReverseLabelLookupFn = std::function<size_t(std::string_view)>;
  using TerminalDefinitionLookupFn = std::function<pmt::util::smrt::GenericAstPath(size_t)>;

 private:
  struct Frame {
    pmt::util::smct::StateMachinePart _sub_part;
    pmt::util::smrt::GenericId::IdType _expression_type;  // Type of expression that this frame is processing
    size_t _stage = 0;                                    // Stage of processing, simulates executing other frames before returning to this one
    size_t _idx = 0;
    size_t _idx_max = 0;
    size_t _idx_chunk = 0;

    size_t _terminal_idx_cur;

    pmt::util::smrt::GenericAstPath _expr_cur_path;  // The expression that this frame is processing

    pmt::util::smct::StateMachinePart _choices;
    pmt::util::smct::StateMachinePart _chunk;
    GrmNumber::RepetitionRangeType _range;
    pmt::util::smrt::StateNrType _state_nr_reference = std::numeric_limits<pmt::util::smrt::StateNrType>::max();

    pmt::util::smct::State* _state_cur = nullptr;
    pmt::util::smct::State* _state_choices = nullptr;
  };

  // -$ Data $-
  pmt::util::smct::StateMachinePart _ret_part;
  pmt::base::IntervalSet<size_t> _terminal_idx_stack_contents;
  std::vector<size_t> _terminal_idx_stack;
  std::vector<Frame> _callstack;

  TerminalLabelLookupFn _fn_lookup_terminal_label;
  TerminalReverseLabelLookupFn _fn_rev_lookup_terminal_label;
  TerminalDefinitionLookupFn _fn_lookup_terminal_definition;
  pmt::util::smrt::GenericAst const* _ast_root = nullptr;
  pmt::util::smct::StateMachine* _dest_state_machine = nullptr;

  bool _keep_current_frame = false;

 public:
  auto build(TerminalLabelLookupFn fn_lookup_terminal_name_, TerminalReverseLabelLookupFn fn_rev_lookup_terminal_name_, TerminalDefinitionLookupFn fn_lookup_terminal_definition_, pmt::util::smrt::GenericAst const& ast_root_, size_t terminal_idx_, pmt::util::smct::StateMachine& dest_state_machine_) -> pmt::util::smct::StateMachinePart;

 private:
  void dispatch(size_t frame_idx_);

  auto build_epsilon() -> pmt::util::smct::StateMachinePart;

  // definition
  void process_definition_stage_0(size_t frame_idx_);

  // sequence
  void process_sequence_stage_0(size_t frame_idx_);
  void process_sequence_stage_1(size_t frame_idx_);

  // choices
  void process_choices_stage_0(size_t frame_idx_);
  void process_choices_stage_1(size_t frame_idx_);
  void process_choices_stage_2(size_t frame_idx_);

  // repetition
  void process_repetition_stage_0(size_t frame_idx_);
  void process_repetition_stage_1(size_t frame_idx_);
  void process_repetition_stage_2(size_t frame_idx_);

  // string_literal
  void process_string_literal_stage_0(size_t frame_idx_);

  // range
  void process_range_stage_0(size_t frame_idx_);

  // integer_literal
  void process_integer_literal_stage_0(size_t frame_idx_);

  // epsilon
  void process_epsilon_stage_0(size_t frame_idx_);

  // terminal_identifier
  void process_terminal_identifier_stage_0(size_t frame_idx_);
  void process_terminal_identifier_stage_1(size_t frame_idx_);
};

}  // namespace pmt::parserbuilder