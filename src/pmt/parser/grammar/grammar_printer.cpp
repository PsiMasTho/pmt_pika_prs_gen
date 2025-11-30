#include "pmt/parser/grammar/grammar_printer.hpp"

#include "parser/clause_base.hpp"
#include "parser/grammar/util.hpp"
#include "pmt/asserts.hpp"
#include "pmt/base/overloaded.hpp"
#include "pmt/parser/grammar/grammar.hpp"
#include "pmt/parser/grammar/rule.hpp"

#include <cassert>
#include <set>
#include <utility>

namespace pmt::parser::grammar {
using namespace pmt::base;
using namespace pmt::util::sm;

namespace {
using ExpressionWithoutParent = RuleExpression const*;

class ExpressionWithParent {
public:
 RuleExpression const* _parent;
 size_t _index = 0;
};

using StackItem = std::variant<std::string, ExpressionWithParent, ExpressionWithoutParent>;

std::string const BOOLALPHA[2]{"false", "true"};  // NOLINT

class Locals {
public:
 std::vector<StackItem> _pending;
};

auto take(Locals& locals_) -> StackItem {
 StackItem const node = std::move(locals_._pending.back());
 locals_._pending.pop_back();
 return node;
}

auto push(Locals& locals_, StackItem item_) {
 locals_._pending.push_back(std::move(item_));
}

void write_rule_lhs_as_grammar(GrammarPrinter::Args& args_, Locals& locals_, std::string const& rule_name_, Rule const& rule_) {
 std::string delim;
 std::string parameter_str;

 if (!rule_._parameters._id_string.empty()) {
  parameter_str += std::exchange(delim, ", ") + "id = \"" + rule_._parameters._id_string + "\"";
 }

 if (rule_._parameters._display_name != rule_name_) {
  parameter_str += std::exchange(delim, ", ") + "display_name = \"" + rule_._parameters._display_name + "\"";
 }

 if (rule_._parameters._merge != RuleParameters::MERGE_DEFAULT) {
  parameter_str += std::exchange(delim, ", ") + "merge = " + BOOLALPHA[rule_._parameters._merge];
 }

 if (rule_._parameters._unpack != RuleParameters::UNPACK_DEFAULT) {
  parameter_str += std::exchange(delim, ", ") + "unpack = " + BOOLALPHA[rule_._parameters._unpack];
 }

 if (!parameter_str.empty()) {
  parameter_str = "<" + parameter_str + ">";
 }

 args_._out << rule_name_ << parameter_str << " = ";
}

auto needs_parens(RuleExpression const* parent_, RuleExpression const* child_) -> bool {
 if (!parent_)
  return false;

 auto const P = parent_->get_tag();
 auto const C = child_->get_tag();
 auto const child_arity = child_->get_children_size();
 auto const is_unary = [&](ClauseBase::Tag t_) {
  return t_ == ClauseBase::Tag::Hidden || t_ == ClauseBase::Tag::NotFollowedBy || t_ == ClauseBase::Tag::OneOrMore;
 };

 switch (C) {
  case ClauseBase::Tag::Sequence:
   // Sequence never needs parens inside Choice or Sequence.
   // It DOES need parens under unary operators if it has multiple items.
   return child_arity > 1 && is_unary(P);

  case ClauseBase::Tag::Choice:
   // Choice needs parens when it's NEXT to other items in a Sequence.
   if (P == ClauseBase::Tag::Sequence)
    return parent_->get_children_size() > 1;
   // Also needs parens under unary operators if it has multiple alts.
   if (is_unary(P))
    return child_arity > 1;
   // Otherwise (Choice inside Choice, etc.) no parens.
   return false;

  default:
   return false;
 }
}

void expand_once(GrammarPrinter::Args& args_, Locals& locals_, RuleExpression const* node_, RuleExpression const* parent_) {
 using Tag = ClauseBase::Tag;

 switch (node_->get_tag()) {
  case Tag::Sequence:
  case Tag::Choice: {
   char const* sep = (node_->get_tag() == Tag::Sequence) ? " " : " | ";
   bool const paren = needs_parens(parent_, node_);

   if (paren)
    push(locals_, ")");  // push close first (stack is LIFO)
   std::string delim;
   for (size_t i = node_->get_children_size(); i--;) {
    push(locals_, std::exchange(delim, sep));
    push(locals_, ExpressionWithParent{._parent = node_, ._index = i});
   }
   if (paren)
    push(locals_, "(");
  } break;
  case Tag::Hidden:
   push(locals_, "~");
   push(locals_, ExpressionWithParent{._parent = node_});
   break;
  case Tag::OneOrMore:
   push(locals_, "+");
   push(locals_, ExpressionWithParent{._parent = node_});
   break;
  case Tag::NotFollowedBy:
   push(locals_, "!");
   push(locals_, ExpressionWithParent{._parent = node_});
   break;
  case Tag::Identifier:
   args_._out << node_->get_identifier();
   break;
  case Tag::CharsetLiteral: {
   args_._out << charset_literal_to_printable_string(node_->get_charset_literal());
  } break;
  case Tag::Epsilon:
   push(locals_, "epsilon");
   break;
 }
}

void write_rule_rhs_as_grammar(GrammarPrinter::Args& args_, Locals& locals_, Rule const& rule_) {
 push(locals_, ";");
 push(locals_, rule_._definition.get());

 while (!locals_._pending.empty()) {
  StackItem const cur = take(locals_);
  std::visit(pmt::base::Overloaded{[&](std::string const& s_) { args_._out << s_; },

                                   [&](ExpressionWithParent expr_) {
                                    if (expr_._parent == nullptr || expr_._parent->get_children_size() <= expr_._index) {
                                     push(locals_, "/* missing expression */");
                                     return;
                                    }
                                    RuleExpression const* child = expr_._parent->get_child_at(expr_._index);
                                    expand_once(args_, locals_, child, expr_._parent);
                                   },

                                   [&](ExpressionWithoutParent expr_) {
                                    if (expr_ == nullptr) {
                                     push(locals_, "/* missing expression */");
                                     return;
                                    }
                                    expand_once(args_, locals_, expr_, nullptr);
                                   }},
             cur);
 }
}

void write_start_rule(GrammarPrinter::Args& args_, Locals& locals_, std::string const& rule_name_) {
 args_._out << "@start = " << (rule_name_.empty() ? "/* missing identifier */" : rule_name_) << ";\n";
}

void write_rule_as_grammar(GrammarPrinter::Args& args_, Locals& locals_, std::string const& rule_name_) {
 Rule const* rule = args_._grammar.get_rule(rule_name_);
 assert(rule != nullptr);
 write_rule_lhs_as_grammar(args_, locals_, rule_name_, *rule);
 write_rule_rhs_as_grammar(args_, locals_, *rule);
}

void write_rule_as_tree(GrammarPrinter::Args& args_, std::string const& rule_name_) {
 args_._out << "Definition: " << rule_name_ << std::endl;

 std::vector<std::pair<RuleExpression const*, size_t>> pending;

 auto const push = [&](RuleExpression const* node_, size_t depth_) {
  pending.emplace_back(node_, depth_);
 };

 auto const take = [&]() {
  auto const ret = pending.back();
  pending.pop_back();
  return ret;
 };

 auto const get_indent = [&](size_t depth_) {
  return std::string(depth_ * 2, ' ');
 };

 push(args_._grammar.get_rule(rule_name_)->_definition.get(), 2);

 while (!pending.empty()) {
  auto const [expr_cur, depth_cur] = take();
  args_._out << get_indent(depth_cur) << ClauseBase::tag_to_string(expr_cur->get_tag());

  switch (expr_cur->get_tag()) {
   case ClauseBase::Tag::CharsetLiteral:
    args_._out << ": " << charset_literal_to_printable_string(expr_cur->get_charset_literal());
    break;
   case ClauseBase::Tag::Identifier:
    args_._out << ": " << expr_cur->get_identifier();
    break;
   case ClauseBase::Tag::Epsilon:
    break;
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::Hidden:
   case ClauseBase::Tag::PegRegular:
   case ClauseBase::Tag::CfgRegular:
   case ClauseBase::Tag::NotFollowedBy:
    args_._out << ": ";
    for (size_t i = expr_cur->get_children_size(); i--;) {
     push(expr_cur->get_child_at(i), depth_cur + 1);
    }
    break;
   default:
    pmt::unreachable();
  }
  args_._out << std::endl;
 }
}

}  // namespace

void GrammarPrinter::print_as_grammar(Args args_) {
 Locals locals;

 std::set<std::string> const rule_names = [&] {
  auto unsorted = args_._grammar.get_rule_names();
  return std::set<std::string>(unsorted.begin(), unsorted.end());
 }();

 write_start_rule(args_, locals, args_._grammar.get_start_rule_name());

 std::string delim;
 for (auto const& rule_name : rule_names) {
  args_._out << std::exchange(delim, "\n");
  write_rule_as_grammar(args_, locals, rule_name);
 }
}

void GrammarPrinter::print_as_tree(Args args_) {
 std::string delim;
 for (std::string const& rule_name : args_._grammar.get_rule_names()) {
  args_._out << std::exchange(delim, "\n");
  write_rule_as_tree(args_, rule_name);
 }
}

}  // namespace pmt::parser::grammar