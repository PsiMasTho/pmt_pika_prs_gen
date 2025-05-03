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
#include <optional>
#include <string>
#include <vector>
#include <deque>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::parserbuilder {

class TerminalStateMachinePartBuilder {
 public:
  // -$ Types / Constants $-
  using TerminalNameLookupFn = std::function<std::optional<std::string>(size_t)>;
  using TerminalReverseNameLookupFn = std::function<std::optional<size_t>(std::string_view)>;
  using TerminalDefinitionLookupFn = std::function<std::optional<pmt::util::smrt::GenericAstPath>(size_t)>;

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
    pmt::util::smct::State* _state_reference = nullptr;
  };

  // -$ Data $-
  pmt::util::smct::StateMachinePart _ret_part;
  pmt::base::IntervalSet<size_t> _terminal_idx_stack_contents;
  std::vector<size_t> _terminal_idx_stack;
  std::deque<Frame> _frames; // Need a stack so that earlier frames are not invalidated on push_back
  std::vector<Frame*> _callstack;

  TerminalNameLookupFn _fn_lookup_terminal_name;
  TerminalReverseNameLookupFn _fn_rev_lookup_terminal_name;
  TerminalDefinitionLookupFn _fn_lookup_terminal_definition;
  pmt::util::smrt::GenericAst const* _ast_root = nullptr;
  pmt::util::smct::StateMachine* _dest_state_machine = nullptr;

 public:
  auto build(TerminalNameLookupFn fn_lookup_terminal_name_, TerminalReverseNameLookupFn fn_rev_lookup_terminal_name_, TerminalDefinitionLookupFn fn_lookup_terminal_definition_, pmt::util::smrt::GenericAst const& ast_root_, size_t terminal_idx_, pmt::util::smct::StateMachine& dest_state_machine_) -> pmt::util::smct::StateMachinePart;

 private:
  void dispatch(Frame& frame_);

  auto build_epsilon() -> pmt::util::smct::StateMachinePart;

  // definition
  void process_definition_stage_0(Frame& frame_);

  // sequence
  void process_sequence_stage_0(Frame& frame_);
  void process_sequence_stage_1(Frame& frame_);

  // choices
  void process_choices_stage_0(Frame& frame_);
  void process_choices_stage_1(Frame& frame_);
  void process_choices_stage_2(Frame& frame_);

  // repetition
  void process_repetition_stage_0(Frame& frame_);
  void process_repetition_stage_1(Frame& frame_);
  void process_repetition_stage_2(Frame& frame_);

  // string_literal
  void process_string_literal_stage_0(Frame& frame_);

  // range
  void process_range_stage_0(Frame& frame_);

  // integer_literal
  void process_integer_literal_stage_0(Frame& frame_);

  // epsilon
  void process_epsilon_stage_0(Frame& frame_);

  // terminal_identifier
  void process_terminal_identifier_stage_0(Frame& frame_);
  void process_terminal_identifier_stage_1(Frame& frame_);
};

}  // namespace pmt::parserbuilder