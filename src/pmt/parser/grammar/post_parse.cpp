#include "pmt/parser/grammar/post_parse.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/match.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/generic_ast_path.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/number.hpp"
#include "pmt/parser/grammar/string_literal.hpp"
#include "pmt/util/sm/primitives.hpp"

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
using namespace pmt::util::sm;

namespace {

class Locals {
 public:
  std::vector<GenericAstPath> _pending;
  std::unordered_map<std::string, std::string> _terminal_direct_display_names_to_names;
  GenericAstPath _path_cur;
  GenericAst* _ast_cur;
  GenericId::IdType _id_cur;
};

auto is_displayable(SymbolValueType symbol_) {
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
          GenericAst::UniqueHandle hidden = GenericAst::construct(GenericAst::Tag::Children, Ast::NtTerminalHidden);
          hidden->give_child_at_back(std::move(item));
          locals_._ast_cur->give_child_at_back(std::move(hidden));
        } break;
        case Ast::NtSequenceModifier: {
          static std::string const ERROR_MSG = "Error: Invalid sequence modifier";
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

void add_terminal_definition(GenericAst& ast_root_, std::string const& terminal_name_, std::string const& terminal_id_, GenericAst const& terminal_definition_) {
  assert(ast_root_.get_id() == GenericId::IdRoot);

  // Production
  GenericAst::UniqueHandle terminal_production = GenericAst::construct(GenericAst::Tag::Children, Ast::NtTerminalProduction);
  // Name
  terminal_production->give_child_at_back(GenericAst::construct(GenericAst::Tag::String, Ast::TkTerminalIdentifier));
  terminal_production->get_child_at_back()->set_string(terminal_name_);
  // Id
  terminal_production->give_child_at_back(GenericAst::construct(GenericAst::Tag::String, Ast::NtParameterId));
  terminal_production->get_child_at_back()->set_string("\"" + terminal_id_ + "\"");
  // Definition
  GenericAst::UniqueHandle terminal_definition = GenericAst::construct(GenericAst::Tag::Children, Ast::NtTerminalDefinition);
  terminal_definition->give_child_at_back(GenericAst::clone(terminal_definition_));
  // Add to production
  terminal_production->give_child_at_back(std::move(terminal_definition));
  // Add to root
  ast_root_.give_child_at_back(std::move(terminal_production));
}

auto direct_terminal_definition_to_display_name(GenericAst const& terminal_definition_) -> std::string {
  switch (terminal_definition_.get_id()) {
    case Ast::TkStringLiteral: {
      return StringLiteral(terminal_definition_).get_value();
    } break;
    case Ast::TkIntegerLiteral: {
      Number const number(terminal_definition_);
      if (is_displayable(number.get_value())) {
        return std::string(1, static_cast<char>(number.get_value()));
      } else {
        std::stringstream ss;
        ss << std::hex << number.get_value();
        std::string ret = ss.str();
        std::transform(ret.begin(), ret.end(), ret.begin(), [](auto const c_) { return std::toupper(c_); });
        return "0x" + ret;
      }
    } break;
    default:
      pmt::unreachable();
  }
}

void handle_direct_terminal(PostParse::Args& args_, Locals& locals_) {
  if (get_cur_topmost_id(args_, locals_) != Ast::NtNonterminalProduction) {
    return;
  }

  std::string const terminal_direct_display_name = direct_terminal_definition_to_display_name(*locals_._ast_cur);

  auto itr = locals_._terminal_direct_display_names_to_names.find(terminal_direct_display_name);
  if (itr == locals_._terminal_direct_display_names_to_names.end()) {
    std::string const terminal_direct_name = Ast::TERMINAL_DIRECT_PREFIX + std::to_string(locals_._terminal_direct_display_names_to_names.size());
    add_terminal_definition(args_._ast_root, terminal_direct_name, GenericId::id_to_string(GenericId::IdDefault), *locals_._ast_cur);
    itr = locals_._terminal_direct_display_names_to_names.emplace(terminal_direct_display_name, terminal_direct_name).first;
  }

  GenericAst::UniqueHandle replacement = GenericAst::construct(GenericAst::Tag::String, Ast::TkTerminalIdentifier);
  replacement->set_string(itr->second);
  GenericAst::swap(*locals_._ast_cur, *replacement);
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
      case GenericId::IdRoot: {
        push_all_children(args_, locals);
      } break;
      case Ast::NtTerminalProduction:
      case Ast::NtNonterminalProduction: {
        push_children_with_ids(args_, locals, {Ast::NtTerminalDefinition, Ast::NtNonterminalDefinition});
      } break;
      case Ast::NtGrammarProperty: {
        push_children_with_ids(args_, locals, {Ast::NtTerminalDefinition, Ast::NtTerminalDefinitionPair});
      } break;
      case Ast::NtTerminalDefinitionPair:
      case Ast::NtTerminalDefinition:
      case Ast::NtNonterminalDefinition:
      case Ast::NtTerminalSequence:
      case Ast::NtNonterminalSequence:
      case Ast::NtTerminalChoices:
      case Ast::NtNonterminalChoices: {
        push_all_children(args_, locals);
      } break;
      case Ast::NtTerminalExpression:
      case Ast::NtNonterminalExpression: {
        handle_expression(args_, locals);
      } break;
      case Ast::NtRepetitionExpression:
      case Ast::NtSequenceModifier: {
        push_nth_child(args_, locals, 0);
      } break;
      case Ast::NtTerminalHidden: {
        push_all_children(args_, locals);
      } break;
      case Ast::TkStringLiteral:
      case Ast::TkIntegerLiteral: {
        handle_direct_terminal(args_, locals);
      } break;
      default:
        continue;
    }
  }
}

}  // namespace pmt::parser::grammar