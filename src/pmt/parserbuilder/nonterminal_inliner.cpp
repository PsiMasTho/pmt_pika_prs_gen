#include "pmt/parserbuilder/nonterminal_inliner.hpp"

#include "pmt/base/interval_set.hpp"
#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/parserbuilder/grammar_data.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

#include <unordered_set>

namespace pmt::parserbuilder {
using namespace pmt::base;
using namespace pmt::util::smrt;

namespace {

auto check_inlineable(NonterminalInliner::Args const& args_, AcceptsIndexType accepts_index_) -> bool {
  std::vector<GenericAst const*> pending;
  std::unordered_set<GenericAst const*> visited;

  auto const push_and_visit = [&](GenericAst const& node_) {
    if (visited.contains(&node_)) {
      return;
    }

    pending.push_back(&node_);
    visited.insert(&node_);
  };

  auto const take = [&]() -> GenericAst const& {
   GenericAst const& ret = *pending.back();
   pending.pop_back();
   return ret;
  };

  push_and_visit(*args_._grammar_data._nonterminal_accepts[accepts_index_]._definition_path.resolve(args_._ast));

  while (!pending.empty()) {
   GenericAst const& node = take();
   switch (node.get_id()) {
    case GrmAst::TkNonterminalIdentifier: {
      AcceptsIndexType const index = args_._grammar_data.lookup_nonterminal_index_by_label(node.get_string());
      if (index == accepts_index_) {
       return false;
      }
      push_and_visit(*args_._grammar_data.lookup_nonterminal_definition_by_index(index).resolve(args_._ast));
    } break;      
    case GrmAst::NtNonterminalDefinition:
    case GrmAst::NtNonterminalChoices:
    case GrmAst::NtNonterminalSequence:
      for (size_t i = 0; i < node.get_children_size(); ++i) {
        push_and_visit(*node.get_child_at(i));
      }
      break;
    case GrmAst::NtNonterminalRepetition:
      push_and_visit(*node.get_child_at(0));
      break;
    default:
      break;
   }
  }

  return true;
}

void perform_inline(NonterminalInliner::Args& args_, AcceptsIndexType accepts_index_) {
  GenericAstPath const definition_path = args_._grammar_data.lookup_nonterminal_definition_by_index(accepts_index_);

  std::vector<GenericAstPath> pending{args_._grammar_data._start_nonterminal_definition};
  std::unordered_set<GenericAstPath> visited{definition_path, args_._grammar_data._start_nonterminal_definition};

  auto const push_and_visit = [&](GenericAstPath const& path_) {
    if (visited.contains(path_)) {
      return;
    }

    pending.push_back(path_);
    visited.insert(path_);
  };

  auto const take = [&]() -> GenericAstPath {
   GenericAstPath const ret = pending.back();
   pending.pop_back();
   return ret;
  };

  for (GrammarData::NonterminalAcceptData const& accept_data : args_._grammar_data._nonterminal_accepts) {
   push_and_visit(accept_data._definition_path);
  }

  while (!pending.empty()) {
   GenericAstPath const path = take();
   GenericAst& node = *path.resolve(args_._ast);

   switch (node.get_id()) {
    case GrmAst::TkNonterminalIdentifier: {
      AcceptsIndexType const index = args_._grammar_data.lookup_nonterminal_index_by_label(node.get_string());
      if (index != accepts_index_) {
       break;
      }
      GenericAstPath const parent_path = path.clone_pop();
      GenericAst& parent_node = *parent_path.resolve(args_._ast);
      parent_node.take_child_at(path.back());
      parent_node.give_child_at(path.back(), GenericAst::clone(*definition_path.resolve(args_._ast)));
      // We changed this expression, so we need to re-visit
      visited.erase(path);
      push_and_visit(parent_path);
    } break;      
    case GrmAst::NtNonterminalDefinition:
    case GrmAst::NtNonterminalChoices:
    case GrmAst::NtNonterminalSequence:
      for (size_t i = 0; i < node.get_children_size(); ++i) {
        push_and_visit(path.clone_push(i));
      }
      break;
    case GrmAst::NtNonterminalRepetition:
      push_and_visit(path.clone_push(0));
      break;
    default:
      break;
   }
  }
}

auto get_nonterminals_to_inline(GrammarData const& grammar_data_) -> IntervalSet<AcceptsIndexType> {
 IntervalSet<StateNrType> ret;
 for (AcceptsIndexType i = 0; i < grammar_data_._nonterminal_accepts.size(); ++i) {
  if (!grammar_data_._nonterminal_accepts[i]._unpack) {
   continue;
  }
  ret.insert(Interval<AcceptsIndexType>(i));
 }

 return ret;
}

}

auto NonterminalInliner::do_inline(Args args_) -> size_t {
 size_t ret = 0;
 get_nonterminals_to_inline(args_._grammar_data).for_each_key([&](AcceptsIndexType const accepts_index_) {
  if (!check_inlineable(args_, accepts_index_)) {
   return;
  }
  perform_inline(args_, accepts_index_);
  ++ret;
 });

 return ret;
}

}