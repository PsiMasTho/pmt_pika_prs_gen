#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"
#include "pmt/util/smrt/generic_id.hpp"
#include "pmt/parserbuilder/grm_number.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::parserbuilder {

class NonterminalStateMachinePartBuilder {
 public:
  // -$ Types / Constants $-
  using NonterminalLabelLookupFn = std::function<std::string(size_t)>;
  using NonterminalReverseLabelLookupFn = std::function<size_t(std::string_view)>;
  using NonterminalDefinitionLookupFn = std::function<pmt::util::smrt::GenericAstPath(size_t)>;
  using TerminalIndexLookupFn = std::function<size_t(std::string_view)>;

 private:
  struct Frame {
    pmt::util::smct::StateMachinePart _sub_part;
    pmt::util::smrt::GenericId::IdType _expression_type;
    pmt::util::smrt::GenericAstPath _expr_cur_path;
    pmt::util::smct::State* _state_cur = nullptr;
    size_t _stage = 0;
    size_t _idx = 0;
    size_t _nonterminal_idx_cur;
    size_t _idx_max = 0;
    size_t _idx_chunk = 0;

    pmt::util::smrt::StateNrType _state_nr_open_cur;

    pmt::util::smct::StateMachinePart _choices;
    pmt::util::smct::StateMachinePart _chunk;
    GrmNumber::RepetitionRangeType _range;

    pmt::util::smct::State* _state_choices = nullptr;
  };

  // -$ Data $-
  pmt::util::smct::StateMachinePart _ret_part;
  std::unordered_map<size_t, pmt::util::smrt::StateNrType> _nonterminal_idx_to_state_nr_post_open;
  std::unordered_map<size_t, pmt::util::smrt::StateNrType> _nonterminal_idx_to_state_nr_close;
  std::vector<Frame> _callstack;

  NonterminalLabelLookupFn _fn_lookup_nonterminal_label;
  NonterminalReverseLabelLookupFn _fn_rev_lookup_nonterminal_label;
  NonterminalDefinitionLookupFn _fn_lookup_nonterminal_definition;
  TerminalIndexLookupFn _fn_lookup_terminal_index;
  pmt::util::smrt::GenericAst const* _ast_root = nullptr;
  pmt::util::smct::StateMachine* _dest_state_machine = nullptr;

  bool _keep_current_frame = false;

 public:
  auto build(NonterminalLabelLookupFn fn_lookup_nonterminal_name_, NonterminalReverseLabelLookupFn fn_rev_lookup_nonterminal_name_, NonterminalDefinitionLookupFn fn_lookup_nonterminal_definition_, TerminalIndexLookupFn fn_lookup_terminal_index_, pmt::util::smrt::GenericAst const& ast_root_, pmt::util::smrt::GenericAstPath start_nonterminal_definition_, pmt::util::smct::StateMachine& dest_state_machine_) -> pmt::util::smct::StateMachinePart;

 private:
  void dispatch(size_t frame_idx_);

  auto build_epsilon() -> pmt::util::smct::StateMachinePart;

  // definition
  void process_definition_stage_0(size_t frame_idx_);
  void process_definition_stage_1(size_t frame_idx_);

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

  // nonterminal_identifier
  void process_nonterminal_identifier_stage_0(size_t frame_idx_);
  void process_nonterminal_identifier_stage_1(size_t frame_idx_);

  // terminal_identifier
  void process_terminal_identifier_stage_0(size_t frame_idx_);

  // epsilon
  void process_epsilon_stage_0(size_t frame_idx_);
};

}  // namespace pmt::parserbuilder