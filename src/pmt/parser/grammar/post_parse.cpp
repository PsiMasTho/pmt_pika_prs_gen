#include "pmt/parser/grammar/post_parse.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/match.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/generic_ast_path.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/number.hpp"
#include "pmt/parser/grammar/string_literal.hpp"
#include "pmt/sm/primitives.hpp"

#include <cassert>
#include <cctype>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pmt::parser::grammar {
using namespace pmt::base;
using namespace pmt::parser;
using namespace pmt::sm;

namespace {

class Locals {
public:
 std::vector<GenericAstPath> _pending;
 std::unordered_map<std::string, std::string> _terminal_direct_display_names_to_names;
 GenericAstPath _path_cur;
 GenericAst* _ast_cur;
 GenericId::IdType _id_cur;
};

auto is_displayable(SymbolType symbol_) {
 switch (symbol_) {
  case '"': {
   return false;
  }
  default: {
   return (symbol_ > UCHAR_MAX) ? false : std::isprint(static_cast<unsigned char>(symbol_)) != 0;
  }
 }
}

auto get_cur_topmost_id(PostParse::Args& args_, Locals& locals_) -> GenericId::IdType {
 return args_._ast_root.get_child_at(locals_._path_cur.front())->get_id();
}

void push(Locals& locals_, GenericAstPath const& path_) {
 locals_._pending.push_back(path_);
}

void push_all_children(PostParse::Args& args_, Locals& locals_) {
 for (size_t i = 0; i < locals_._ast_cur->get_children_size(); ++i) {
  push(locals_, locals_._path_cur.clone_push(i));
 }
}

void push_nth_child(PostParse::Args& args_, Locals& locals_, size_t n_) {
 if (n_ >= locals_._ast_cur->get_children_size()) {
  return;
 }
 push(locals_, locals_._path_cur.clone_push(n_));
}

void push_children_with_ids(PostParse::Args& args_, Locals& locals_, std::unordered_set<GenericId::IdType> const& ids_) {
 for (size_t i = 0; i < locals_._ast_cur->get_children_size(); ++i) {
  GenericAst const& child = *locals_._ast_cur->get_child_at(i);
  if (ids_.contains(child.get_id())) {
   push(locals_, locals_._path_cur.clone_push(i));
  }
 }
}

void handle_expression(PostParse::Args& args_, Locals& locals_) {
 switch (locals_._ast_cur->get_children_size()) {
  case 2: {
   switch (locals_._ast_cur->get_child_at_back()->get_id()) {
    case Ast::TkTilde: {
     GenericAst::UniqueHandle item = locals_._ast_cur->take_child_at_front();
     locals_._ast_cur->take_child_at_front();
     GenericAst::UniqueHandle hidden = GenericAst::construct(GenericAst::Tag::Children, Ast::NtHidden);
     hidden->give_child_at_back(std::move(item));
     locals_._ast_cur->give_child_at_back(std::move(hidden));
    } break;
    case Ast::NtAdvancedExpression: {
     static std::string const ERROR_MSG = "Error: Invalid advanced expression";
     if (Match::is_none_of(locals_._ast_cur->get_child_at(0)->get_id(), Ast::NtTerminalChoices, Ast::NtNonterminalChoices) || locals_._ast_cur->get_child_at(0)->get_children_size() != 1 || Match::is_none_of(locals_._ast_cur->get_child_at(0)->get_child_at(0)->get_id(), Ast::NtTerminalSequence, Ast::NtNonterminalSequence)) {
      throw std::runtime_error(ERROR_MSG);
     }

     switch (locals_._ast_cur->get_child_at_back()->get_child_at_front()->get_id()) {
      case Ast::TkPermute: {
       locals_._ast_cur->get_child_at_back()->set_id(Ast::NtPermute);
      } break;
      case Ast::TkPermuteDelimited: {
       locals_._ast_cur->get_child_at_back()->set_id(Ast::NtPermuteDelimited);
      } break;
      case Ast::TkIntersect: {
       locals_._ast_cur->get_child_at_back()->set_id(Ast::NtIntersect);
      } break;
      default: {
       pmt::unreachable();
      }
     };

     locals_._ast_cur->get_child_at_back()->take_child_at_front();

     locals_._ast_cur->unpack(0);
    };
    case Ast::NtRepetitionExpression: {
     GenericAst::UniqueHandle item = locals_._ast_cur->take_child_at_front();
     locals_._ast_cur->get_child_at_front()->give_child_at_front(std::move(item));
    };
   }
  };
  case 1: {
   push_all_children(args_, locals_);
  }
  case 0: {
  } break;
  default: {
   pmt::unreachable();
  }
 }
}

auto take(Locals& locals_) -> GenericAstPath {
 auto ret = locals_._pending.back();
 locals_._pending.pop_back();
 return ret;
}

}  // namespace

void PostParse::transform(Args args_) {
 Locals locals;

 push(locals, {});

 while (!locals._pending.empty()) {
  locals._path_cur = take(locals);
  locals._ast_cur = locals._path_cur.resolve(args_._ast_root);
  locals._id_cur = locals._ast_cur->get_id();

  switch (locals._id_cur) {
   case GenericId::IdRoot:
   case Ast::NtTerminalDefinitionPair:
   case Ast::NtTerminalDefinition:
   case Ast::NtNonterminalDefinition:
   case Ast::NtTerminalSequence:
   case Ast::NtNonterminalSequence:
   case Ast::NtTerminalChoices:
   case Ast::NtRepetitionExpression:
   case Ast::NtAdvancedExpression:
   case Ast::NtHidden:
   case Ast::NtNonterminalChoices:
   case Ast::NtPermute:
   case Ast::NtPermuteDelimited: {
    push_all_children(args_, locals);
   } break;
   case Ast::NtTerminalProduction:
   case Ast::NtNonterminalProduction: {
    push_children_with_ids(args_, locals, {Ast::NtTerminalDefinition, Ast::NtNonterminalDefinition});
   } break;
   case Ast::NtGrammarProperty: {
    push_children_with_ids(args_, locals, {Ast::NtTerminalDefinition, Ast::NtTerminalDefinitionPair});
   } break;
   case Ast::NtTerminalExpression:
   case Ast::NtNonterminalExpression: {
    handle_expression(args_, locals);
   } break;
   case Ast::TkStringLiteral:
   case Ast::TkIntegerLiteral: {
    // handle_direct_terminal(args_, locals);
   } break;
   default:
    continue;
  }
 }
}

}  // namespace pmt::parser::grammar