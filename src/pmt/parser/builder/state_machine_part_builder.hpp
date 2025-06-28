#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parser/generic_ast_path.hpp"
#include "pmt/parser/grammar/number.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/ct/state_machine_part.hpp"

#include <functional>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::util::sm::ct, StateMachine)

namespace pmt::parser::builder {

class StateMachinePartBuilder {
public:
 // -$ Types / Constants $-
 using LabelLookupFn = std::function<std::string(size_t)>;
 using DefinitionLookupFn = std::function<GenericAstPath(size_t)>;
 using ReverseLabelLookupFn = std::function<size_t(std::string_view)>;

 class ArgsBase {
 public:
  GenericAst const& _ast_root;
  pmt::util::sm::ct::StateMachine& _state_machine_dest;
  DefinitionLookupFn _fn_lookup_definition;
  size_t _starting_index;
 };

 class TerminalBuildingArgs : public ArgsBase {
 public:
  TerminalBuildingArgs(ArgsBase base_, LabelLookupFn fn_lookup_terminal_label_, ReverseLabelLookupFn fn_rev_lookup_terminal_label_);
  LabelLookupFn _fn_lookup_terminal_label;
  ReverseLabelLookupFn _fn_rev_lookup_terminal_label;
 };

 class NonterminalBuildingArgs : public ArgsBase {
 public:
  NonterminalBuildingArgs(ArgsBase base_, LabelLookupFn fn_lookup_nonterminal_label_, ReverseLabelLookupFn fn_rev_lookup_nonterminal_label_, ReverseLabelLookupFn fn_rev_lookup_terminal_label_);
  LabelLookupFn _fn_lookup_nonterminal_label;
  ReverseLabelLookupFn _fn_rev_lookup_nonterminal_label;
  ReverseLabelLookupFn _fn_rev_lookup_terminal_label;
 };

 // --$ Other $--
 static auto build(TerminalBuildingArgs const& args_) -> pmt::util::sm::ct::StateMachinePart;
 static auto build(NonterminalBuildingArgs const& args_) -> pmt::util::sm::ct::StateMachinePart;
};

}  // namespace pmt::parser::builder