#include "pmt/meta/grammar_to_str.hpp"

#include "pmt/meta/grammar.hpp"
#include "pmt/meta/literal_to_str.hpp"
#include "pmt/meta/rule.hpp"
#include "pmt/rt/clause_base.hpp"
#include "pmt/util/overloaded.hpp"

#include <cassert>
#include <set>
#include <utility>

namespace pmt::meta {
using namespace pmt::rt;

namespace {
using ExpressionWithoutParent = RuleExpression const*;

class ExpressionWithParent {
public:
 RuleExpression const* _parent;
 size_t _index = 0;
};

using StackItem = std::variant<std::string, ExpressionWithParent, ExpressionWithoutParent>;

char const* const BOOLALPHA[2]{"false", "true"};

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

auto rule_lhs_to_str(Locals& locals_, std::string const& rule_name_, Rule const& rule_) -> std::string {
 std::string delim;
 std::string parameter_str;

 if (rule_._parameters._id_string != AstId::id_to_string(AstId::IdDefault)) {
  parameter_str += std::exchange(delim, ", ") + "id = \"" + rule_._parameters._id_string + "\"";
 }

 if (rule_._parameters._display_name != rule_name_) {
  parameter_str += std::exchange(delim, ", ") + "display_name = \"" + rule_._parameters._display_name + "\"";
 }

 if (rule_._parameters._merge != RuleParametersBase::MERGE_DEFAULT) {
  parameter_str += std::exchange(delim, ", ") + "merge = " + BOOLALPHA[rule_._parameters._merge];
 }

 if (rule_._parameters._unpack != RuleParametersBase::UNPACK_DEFAULT) {
  parameter_str += std::exchange(delim, ", ") + "unpack = " + BOOLALPHA[rule_._parameters._unpack];
 }

 if (rule_._parameters._hide != RuleParametersBase::HIDE_DEFAULT) {
  parameter_str += std::exchange(delim, ", ") + "hide = " + BOOLALPHA[rule_._parameters._hide];
 }

 if (!parameter_str.empty()) {
  parameter_str = "<" + parameter_str + ">";
 }

 return "$" + rule_name_ + parameter_str + " = ";
}

auto needs_parens(RuleExpression const* parent_, RuleExpression const* child_) -> bool {
 if (!parent_) {
  return false;
 }

 ClauseBase::Tag const tag_parent = parent_->get_tag();
 ClauseBase::Tag const tag_child = child_->get_tag();
 size_t const arity_child = child_->get_children_size();
 auto const is_unary = [&](ClauseBase::Tag tag_) {
  return tag_ == ClauseBase::Tag::NegativeLookahead || tag_ == ClauseBase::Tag::OneOrMore;
 };

 switch (tag_child) {
  case ClauseBase::Tag::Sequence:
   // Sequence never needs parens inside Choice or Sequence.
   // It DOES need parens under unary operators if it has multiple items.
   return arity_child > 1 && is_unary(tag_parent);
  case ClauseBase::Tag::Choice:
   // Choice needs parens when it's NEXT to other items in a Sequence.
   if (tag_parent == ClauseBase::Tag::Sequence) {
    return parent_->get_children_size() > 1;
   }
   // Also needs parens under unary operators if it has multiple alts.
   if (is_unary(tag_parent)) {
    return arity_child > 1;
   }
   // Otherwise (Choice inside Choice, etc.) no parens.
   return false;
  case ClauseBase::Tag::NegativeLookahead:
  case ClauseBase::Tag::OneOrMore: {
   if (is_unary(tag_parent)) {
    return true;
   }
  }
  default:
   return false;
 }
}

auto expand_once(Locals& locals_, RuleExpression const* node_, RuleExpression const* parent_) -> std::string {
 bool const paren = needs_parens(parent_, node_);
 paren ? push(locals_, ")") : void();

 std::string ret;
 switch (node_->get_tag()) {
  case ClauseBase::Tag::Sequence:
  case ClauseBase::Tag::Choice: {
   std::string const sep = (node_->get_tag() == ClauseBase::Tag::Sequence) ? " " : " | ";
   std::string delim;
   for (size_t i = node_->get_children_size(); i--;) {
    push(locals_, std::exchange(delim, sep));
    push(locals_, ExpressionWithParent{._parent = node_, ._index = i});
   }
  } break;
  case ClauseBase::Tag::OneOrMore: {
   push(locals_, "+");
   push(locals_, ExpressionWithParent{._parent = node_});
  } break;
  case ClauseBase::Tag::NegativeLookahead: {
   push(locals_, "!");
   push(locals_, ExpressionWithParent{._parent = node_});
  } break;
  case ClauseBase::Tag::Identifier: {
   ret += "$" + node_->get_identifier();
  } break;
  case ClauseBase::Tag::CharsetLiteral: {
   ret += charset_literal_to_grammar_string(node_->get_charset_literal());
  } break;
  case ClauseBase::Tag::Epsilon: {
   push(locals_, "epsilon");
  } break;
 }

 paren ? push(locals_, "(") : void();

 return ret;
}

auto rule_rhs_to_str(Locals& locals_, Rule const& rule_) -> std::string {
 push(locals_, ";");
 push(locals_, rule_._definition.get());

 std::string ret;
 while (!locals_._pending.empty()) {
  StackItem const cur = take(locals_);
  std::visit(pmt::util::Overloaded{[&](std::string const& s_) { ret += s_; },
                                   [&](ExpressionWithParent expr_) {
                                    if (expr_._parent == nullptr || expr_._parent->get_children_size() <= expr_._index) {
                                     push(locals_, "/* missing expression */");
                                     return;
                                    }
                                    RuleExpression const* child = expr_._parent->get_child_at(expr_._index);
                                    ret += expand_once(locals_, child, expr_._parent);
                                   },

                                   [&](ExpressionWithoutParent expr_) {
                                    if (expr_ == nullptr) {
                                     push(locals_, "/* missing expression */");
                                     return;
                                    }
                                    ret += expand_once(locals_, expr_, nullptr);
                                   }},
             cur);
 }

 return ret;
}

auto start_rule_to_str(std::string const& rule_name_) -> std::string {
 return "@start = " + (rule_name_.empty() ? "/* missing identifier */" : "$" + rule_name_) + ";\n";
}

auto rule_to_str(Grammar const& grammar_, Locals& locals_, std::string const& rule_name_) -> std::string {
 Rule const* rule = grammar_.get_rule(rule_name_);
 assert(rule != nullptr);
 return rule_lhs_to_str(locals_, rule_name_, *rule) + rule_rhs_to_str(locals_, *rule);
}

}  // namespace

auto grammar_to_string(Grammar const& grammar_) -> std::string {
 Locals locals;

 std::set<std::string> const rule_names = [&] {
  auto unsorted = grammar_.get_rule_names();
  return std::set<std::string>(unsorted.begin(), unsorted.end());
 }();

 std::string ret = start_rule_to_str(grammar_.get_start_rule_name());

 std::string delim;
 for (auto const& rule_name : rule_names) {
  ret += std::exchange(delim, "\n") + rule_to_str(grammar_, locals, rule_name);
 }

 return ret;
}

}  // namespace pmt::meta