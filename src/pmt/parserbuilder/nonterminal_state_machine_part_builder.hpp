#pragma once

#include "pmt/fw_decl.hpp"
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

class NonterminalStateMachinePartBuilder {
 public:
  // -$ Types / Constants $-
  using NonterminalLabelLookupFn = std::function<std::optional<std::string>(size_t)>;
  using NonterminalReverseLabelLookupFn = std::function<std::optional<size_t>(std::string_view)>;
  using NonterminalDefinitionLookupFn = std::function<std::optional<pmt::util::smrt::GenericAstPath>(size_t)>;

 private:
  struct Frame {
    pmt::util::smct::StateMachinePart _sub_part;
    pmt::util::smrt::GenericId::IdType _expression_type;
    pmt::util::smrt::GenericAstPath _expr_cur_path;
    size_t _stage = 0;
    size_t _idx = 0;
  };

  // -$ Data $-
  pmt::util::smct::StateMachinePart _ret_part;
  std::vector<size_t> _nonterminal_idx_stack;
  std::deque<Frame> _frames;
  std::vector<Frame*> _callstack;

  NonterminalLabelLookupFn _fn_lookup_nonterminal_label;
  NonterminalReverseLabelLookupFn _fn_rev_lookup_nonterminal_label;
  NonterminalDefinitionLookupFn _fn_lookup_nonterminal_definition;
  pmt::util::smrt::GenericAst const* _ast_root = nullptr;
  pmt::util::smct::StateMachine* _dest_state_machine = nullptr;

 public:
  auto build(NonterminalLabelLookupFn fn_lookup_nonterminal_name_, NonterminalReverseLabelLookupFn fn_rev_lookup_nonterminal_name_, NonterminalDefinitionLookupFn fn_lookup_nonterminal_definition_, pmt::util::smrt::GenericAst const& ast_root_, size_t nonterminal_idx_, pmt::util::smct::StateMachine& dest_state_machine_) -> pmt::util::smct::StateMachinePart;

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

  // nonterminal_identifier
  void process_nonterminal_identifier_stage_0(Frame& frame_);
  void process_nonterminal_identifier_stage_1(Frame& frame_);
};

}  // namespace pmt::parserbuilder