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
 _callstack.clear();

 _dest_state_machine = &dest_state_machine_;

 _nonterminal_idx_stack.push_back(nonterminal_idx_);

 _frames.emplace_back();
 _frames.back()._expr_cur_path = *_fn_lookup_nonterminal_definition(nonterminal_idx_);
 _frames.back()._expression_type = _frames.back()._expr_cur_path.resolve(*_ast_root)->get_id();
 _callstack.push_back(&_frames.back());

 while (!_callstack.empty()) {
   Frame* cur = _callstack.back();
   _callstack.pop_back();
   dispatch(*cur);
 }

 // cleanup in case this object is used for another build
 _frames.clear();

 return std::move(_ret_part);
}

void NonterminalStateMachinePartBuilder::dispatch(Frame& frame_){

}

auto NonterminalStateMachinePartBuilder::build_epsilon() -> pmt::util::smct::StateMachinePart{

}

void NonterminalStateMachinePartBuilder::process_definition_stage_0(Frame& frame_){

}

void NonterminalStateMachinePartBuilder::process_sequence_stage_0(Frame& frame_){

}

void NonterminalStateMachinePartBuilder::process_sequence_stage_1(Frame& frame_){

}

void NonterminalStateMachinePartBuilder::process_choices_stage_0(Frame& frame_){

}

void NonterminalStateMachinePartBuilder::process_choices_stage_1(Frame& frame_){

}

void NonterminalStateMachinePartBuilder::process_choices_stage_2(Frame& frame_){

}

void NonterminalStateMachinePartBuilder::process_nonterminal_identifier_stage_0(Frame& frame_){

}

void NonterminalStateMachinePartBuilder::process_nonterminal_identifier_stage_1(Frame& frame_){

}


}