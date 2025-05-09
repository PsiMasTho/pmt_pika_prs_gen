#include "pmt/parserbuilder/nonterminal_state_machine_part_builder.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smct;
using namespace pmt::util::smrt;

namespace {

}

auto NonterminalStateMachinePartBuilder::build(NonterminalLabelLookupFn fn_lookup_nonterminal_name_, NonterminalReverseLabelLookupFn fn_rev_lookup_nonterminal_name_, NonterminalDefinitionLookupFn fn_lookup_nonterminal_definition_, pmt::util::smrt::GenericAst const& ast_root_, size_t nonterminal_idx_, pmt::util::smct::StateMachine& dest_state_machine_) -> pmt::util::smct::StateMachinePart{
 _fn_lookup_nonterminal_label = std::move(fn_lookup_nonterminal_name_);
 _fn_rev_lookup_nonterminal_label = std::move(fn_rev_lookup_nonterminal_name_);
 _fn_lookup_nonterminal_definition = std::move(fn_lookup_nonterminal_definition_);
 _ast_root = &ast_root_;
 _nonterminal_idx_stack.clear();

 _dest_state_machine = &dest_state_machine_;

 _nonterminal_idx_stack.push_back(nonterminal_idx_);

 _callstack.emplace_back();
 _callstack.back()._expr_cur_path = *_fn_lookup_nonterminal_definition(nonterminal_idx_);
 _callstack.back()._expression_type = _callstack.back()._expr_cur_path.resolve(*_ast_root)->get_id();

 while (!_callstack.empty()) {
  _keep_current_frame = false; 
  size_t const frame_idx = _callstack.size() - 1;
  dispatch(frame_idx);
  if (!_keep_current_frame) {
   _callstack.erase(_callstack.begin() + frame_idx);
  }
 }

 return std::move(_ret_part);
}

void NonterminalStateMachinePartBuilder::dispatch(size_t frame_idx_){
 switch (_callstack[frame_idx_]._expression_type) {
  case GrmAst::NtNonterminalDefinition:
    switch (_callstack[frame_idx_]._stage) {
      case 0:
        process_definition_stage_0(frame_idx_);
        break;
      default:
        pmt::unreachable();
    }
    break;
  case GrmAst::NtNonterminalSequence:
    switch (_callstack[frame_idx_]._stage) {
      case 0:
        process_sequence_stage_0(frame_idx_);
        break;
      case 1:
        process_sequence_stage_1(frame_idx_);
        break;
      default:
        pmt::unreachable();
    }
    break;
  case GrmAst::NtNonterminalChoices:
    switch (_callstack[frame_idx_]._stage) {
      case 0:
        process_choices_stage_0(frame_idx_);
        break;
      case 1:
        process_choices_stage_1(frame_idx_);
        break;
      case 2:
        process_choices_stage_2(frame_idx_);
        break;
      default:
        pmt::unreachable();
    }
    break;
  case GrmAst::TkNonterminalIdentifier:
    default: {
     pmt::unreachable();
    }
  }
  
}

auto NonterminalStateMachinePartBuilder::build_epsilon() -> pmt::util::smct::StateMachinePart{

}

void NonterminalStateMachinePartBuilder::process_definition_stage_0(size_t frame_idx_){

}

void NonterminalStateMachinePartBuilder::process_sequence_stage_0(size_t frame_idx_){

}

void NonterminalStateMachinePartBuilder::process_sequence_stage_1(size_t frame_idx_){

}

void NonterminalStateMachinePartBuilder::process_choices_stage_0(size_t frame_idx_){

}

void NonterminalStateMachinePartBuilder::process_choices_stage_1(size_t frame_idx_){

}

void NonterminalStateMachinePartBuilder::process_choices_stage_2(size_t frame_idx_){

}

void NonterminalStateMachinePartBuilder::process_nonterminal_identifier_stage_0(size_t frame_idx_){

}

void NonterminalStateMachinePartBuilder::process_nonterminal_identifier_stage_1(size_t frame_idx_){

}


}