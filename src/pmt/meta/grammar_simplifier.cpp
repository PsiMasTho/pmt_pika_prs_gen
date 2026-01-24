#include "pmt/meta/grammar_simplifier.hpp"

#include "pmt/meta/grammar.hpp"
#include "pmt/meta/rule.hpp"
#include "pmt/rt/clause_base.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/overloaded.hpp"

#include <unordered_set>

namespace pmt::meta {
using namespace pmt::rt;

namespace {

auto get_referenced_rules(RuleExpression const* rule_expression_) -> std::unordered_set<std::string> {
 std::unordered_set<std::string> ret;

 std::vector<RuleExpression const*> pending{rule_expression_};

 auto take = [&]() {
  RuleExpression const* ret = pending.back();
  pending.pop_back();
  return ret;
 };

 while (!pending.empty()) {
  RuleExpression const* cur = take();
  if (cur == nullptr) {
   continue;
  }

  if (cur->get_tag() == ClauseBase::Tag::Identifier) {
   ret.insert(cur->get_identifier());
   continue;
  }

  for (size_t i = 0; i < cur->get_children_size(); ++i) {
   pending.push_back(cur->get_child_at(i));
  }
 }

 return ret;
}

auto get_unpackable_rules(Grammar& grammar_) -> std::unordered_set<std::string> {
 std::unordered_set<std::string> ret = grammar_.get_rule_names();

 std::string const& start_rule_name = grammar_.get_start_rule_name();
 std::vector<std::pair<RuleExpression const*, std::unordered_set<std::string>>> pending{{grammar_.get_rule(start_rule_name)->_definition.get(), {start_rule_name}}};
 std::unordered_set<std::string> visited{grammar_.get_start_rule_name()};

 while (!pending.empty()) {
  auto [expr_cur, visited_cur] = pending.back();
  pending.pop_back();

  switch (expr_cur->get_tag()) {
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::CharsetLiteral:
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::NegativeLookahead: {
    for (size_t i = 0; i < expr_cur->get_children_size(); ++i) {
     pending.emplace_back(expr_cur->get_child_at(i), visited_cur);
    }
   } break;
   case ClauseBase::Tag::Identifier: {
    std::string const rule_name_cur = expr_cur->get_identifier();
    Rule const* rule = grammar_.get_rule(rule_name_cur);
    if (rule == nullptr || !rule->_parameters.get_unpack() || rule->_parameters.get_merge() || rule->_parameters.get_hide()) {
     ret.erase(rule_name_cur);
    }

    if (visited_cur.contains(rule_name_cur)) {
     ret.erase(rule_name_cur);
    } else if (visited.insert(rule_name_cur).second) {
     visited_cur.insert(rule_name_cur);
     pending.emplace_back(rule->_definition.get(), visited_cur);
    }
   } break;
   case ClauseBase::Tag::Epsilon:
    break;
   default:
    pmt::unreachable();
  }
 }

 return ret;
}

void prune(Grammar& grammar_) {
 std::unordered_set<std::string> all_rule_names = grammar_.get_rule_names();

 std::vector<std::string> pending;

 auto push_and_visit = [&](std::string const& rule_name_) {
  if (all_rule_names.erase(rule_name_) == 0) {
   return;
  }

  pending.push_back(rule_name_);
 };

 auto take = [&]() {
  std::string ret = pending.back();
  pending.pop_back();
  return ret;
 };

 push_and_visit(grammar_.get_start_rule_name());

 while (!pending.empty()) {
  std::string const cur_rule_name = take();
  Rule const* cur_rule = grammar_.get_rule(cur_rule_name);
  if (cur_rule == nullptr) {
   continue;
  }
  std::unordered_set<std::string> const referenced_rules = get_referenced_rules(cur_rule->_definition.get());
  for (std::string const& rule_name : referenced_rules) {
   push_and_visit(rule_name);
  }
 }

 // All remaining rule names need to be pruned
 for (std::string const& rule_name : all_rule_names) {
  grammar_.remove_rule(rule_name);
 }
}

void unpack(Grammar& grammar_) {
 struct ExpressionPosition {
  RuleExpression* _parent;
  size_t _child_idx;
 };

 using Expression = std::variant<RuleExpression::UniqueHandle*, ExpressionPosition>;

 std::unordered_set<std::string> const unpackable_rule_names = get_unpackable_rules(grammar_);
 std::string const& start_rule_name = grammar_.get_start_rule_name();
 std::vector<std::pair<Expression, std::unordered_set<std::string>>> pending{{&grammar_.get_rule(start_rule_name)->_definition, {start_rule_name}}};

 while (!pending.empty()) {
  auto [expr_cur_variant, visited_cur] = pending.back();
  pending.pop_back();

  // clang-format off
  std::visit(pmt::util::Overloaded{[&](RuleExpression::UniqueHandle* expr_cur_) {
    switch (expr_cur_->get()->get_tag()) {
     case ClauseBase::Tag::Identifier: {
      std::string rule_name = expr_cur_->get()->get_identifier();
      Rule* const rule_next = grammar_.get_rule(rule_name);
      if (unpackable_rule_names.contains(rule_name)) {
       *expr_cur_ = RuleExpression::clone(*rule_next->_definition);
       if (visited_cur.insert(rule_name).second) {
        pending.emplace_back(expr_cur_, visited_cur);
       }
      } else {
       if (visited_cur.insert(rule_name).second) {
        pending.emplace_back(&rule_next->_definition, visited_cur);
       }
      }
     } break;
     case ClauseBase::Tag::Sequence:
     case ClauseBase::Tag::Choice:
     case ClauseBase::Tag::NegativeLookahead:
     case ClauseBase::Tag::OneOrMore: {
      for (size_t i = 0; i < expr_cur_->get()->get_children_size(); ++i) {
       pending.emplace_back(ExpressionPosition{._parent = expr_cur_->get(), ._child_idx = i}, visited_cur);
      }
     } break;
     case ClauseBase::Tag::CharsetLiteral:
     case ClauseBase::Tag::Epsilon:
      break;
    }
   },
   [&](ExpressionPosition expr_position_cur_) {
    RuleExpression& expr_cur = *expr_position_cur_._parent->get_child_at(expr_position_cur_._child_idx);
    switch (expr_cur.get_tag()) {
     case ClauseBase::Tag::Identifier: {
      std::string rule_name = expr_cur.get_identifier();
      Rule* const rule_next = grammar_.get_rule(rule_name);
      if (unpackable_rule_names.contains(rule_name)) {
       expr_position_cur_._parent->take_child_at(expr_position_cur_._child_idx);
       expr_position_cur_._parent->give_child_at(expr_position_cur_._child_idx, RuleExpression::clone(*rule_next->_definition));
       if (visited_cur.insert(rule_name).second) {
        pending.emplace_back(expr_position_cur_, visited_cur);
       }
      } else {
       if (visited_cur.insert(rule_name).second) {
        pending.emplace_back(&rule_next->_definition, visited_cur);
       }
      }
     } break;
     case ClauseBase::Tag::Sequence:
     case ClauseBase::Tag::Choice:
     case ClauseBase::Tag::NegativeLookahead:
     case ClauseBase::Tag::OneOrMore: {
      for (size_t i = 0; i < expr_cur.get_children_size(); ++i) {
       pending.emplace_back(ExpressionPosition{._parent = &expr_cur, ._child_idx = i}, visited_cur);
      }
     } break;
     case ClauseBase::Tag::CharsetLiteral:
     case ClauseBase::Tag::Epsilon:
      break;
    }
   }},
   expr_cur_variant);
  // clang-format on
 }
}

void flatten_expression(RuleExpression::UniqueHandle& rule_expression_) {
 class StackItem {
 public:
  RuleExpression* _parent = nullptr;
  size_t _idx;
 };

 {
  RuleExpression::UniqueHandle tmp = RuleExpression::construct(ClauseBase::Tag::Choice);
  tmp->give_child_at_back(std::move(rule_expression_));
  rule_expression_ = std::move(tmp);
 }

 bool repeat = false;

 do {
  repeat = false;
  std::vector<StackItem> pending;

  for (size_t j = 0; j < rule_expression_->get_children_size(); ++j) {
   pending.emplace_back(rule_expression_.get(), j);
  }

  while (!pending.empty()) {
   auto [parent, idx] = pending.back();
   pending.pop_back();
   RuleExpression* const child = parent->get_child_at(idx);

   switch (child->get_tag()) {
    case ClauseBase::Tag::Choice:
     if (child->get_children_size() == 1) {
      RuleExpression::UniqueHandle grandchild = child->take_child_at_front();
      parent->take_child_at(idx);
      parent->give_child_at(idx, std::move(grandchild));
      pending.emplace_back(parent, idx);
      repeat = true;
     } else {
      // Any choices in choices can be immediately flattened
      for (size_t j = 0; j < child->get_children_size(); ++j) {
       if (child->get_child_at(j)->get_tag() != ClauseBase::Tag::Choice) {
        continue;
       }

       RuleExpression::UniqueHandle grandchild = child->take_child_at(j);
       for (size_t k = 0; grandchild->get_children_size() > 0; ++k) {
        child->give_child_at(j + k, grandchild->take_child_at_front());
       }
       j = 0;
       repeat = true;
      }

      for (size_t j = 0; j < child->get_children_size(); ++j) {
       pending.emplace_back(child, j);
      }
     }
     break;
    case ClauseBase::Tag::Sequence:
     // Unpack any nested sequences
     for (size_t j = 0; j < child->get_children_size(); ++j) {
      RuleExpression* const grandchild = child->get_child_at(j);
      if (grandchild->get_tag() != ClauseBase::Tag::Sequence) {
       continue;
      }
      child->unpack(j);
      repeat = true;
      j = 0;
     }

     // Remove any epsilons which are not the only child
     for (size_t i = 1; i < child->get_children_size();) {
      if (child->get_child_at(i)->get_tag() == ClauseBase::Tag::Epsilon) {
       child->take_child_at(i);
      } else {
       ++i;
      }
     }
     if (child->get_children_size() > 1 && child->get_child_at_front()->get_tag() == ClauseBase::Tag::Epsilon) {
      child->take_child_at_front();
     }
     if (child->get_children_size() == 1) {
      RuleExpression::UniqueHandle grandchild = child->take_child_at_back();
      parent->take_child_at(idx);
      parent->give_child_at(idx, std::move(grandchild));
      pending.emplace_back(parent, idx);
      repeat = true;
     } else {
      for (size_t j = 0; j < child->get_children_size(); ++j) {
       pending.emplace_back(child, j);
      }
     }
     break;
    case ClauseBase::Tag::OneOrMore:
    case ClauseBase::Tag::NegativeLookahead:
     pending.emplace_back(child, 0);
     break;
    default:
     break;
   }
  }
 } while (repeat);

 // A definition comes in as a ClauseBase::Tag::Choice, if it has one child we can just unpack it
 if (rule_expression_->get_tag() == ClauseBase::Tag::Choice && rule_expression_->get_children_size() == 1) {
  rule_expression_ = rule_expression_->take_child_at_back();
 }
}

void flatten(Grammar& grammar_) {
 for (std::string const& rule_name : grammar_.get_rule_names()) {
  Rule* const rule = grammar_.get_rule(rule_name);
  if (rule == nullptr || rule->_definition == nullptr) {
   continue;
  }

  flatten_expression(rule->_definition);
 }
}
}  // namespace

void GrammarSimplifier::simplify(Grammar& grammar_) {
 unpack(grammar_);
 prune(grammar_);
 flatten(grammar_);
}

}  // namespace pmt::meta