#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/grm_number.hpp"
#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/smct/state_machine_part.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"

#include <functional>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::util::smct, StateMachine)

namespace pmt::parserbuilder {

class StateMachinePartBuilder {
 public:
  // -$ Types / Constants $-
  using LabelLookupFn = std::function<std::string(size_t)>;
  using DefinitionLookupFn = std::function<pmt::util::smrt::GenericAstPath(size_t)>;
  using ReverseLabelLookupFn = std::function<size_t(std::string_view)>;

  class ArgsBase {
   public:
   pmt::util::smrt::GenericAst const& _ast_root;
   pmt::util::smct::StateMachine& _dest_state_machine;
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
  static auto build(TerminalBuildingArgs const& args_) -> pmt::util::smct::StateMachinePart;
  static auto build(NonterminalBuildingArgs const& args_) -> pmt::util::smct::StateMachinePart;
};

}  // namespace pmt::parserbuilder