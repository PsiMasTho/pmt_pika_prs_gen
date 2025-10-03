#include "pmt/parser/grammar/grammar_simplifier.hpp"
#include <unordered_set>

#include "parser/grammar/grammar.hpp"
#include "parser/grammar/rule.hpp"

namespace pmt::parser::grammar {

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

  if (cur->get_tag() == RuleExpression::Tag::Identifier) {
   ret.insert(cur->get_identifier());
   continue;
  }

  for (size_t i = 0; i < cur->get_children_size(); ++i) {
   pending.push_back(cur->get_child_at(i));
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

void flatten_expression(RuleExpression* rule_expression_) {
 bool repeat = false;

 do {
  repeat = false;
  std::vector<std::pair<RuleExpression*, size_t>> pending;
  pending.emplace_back(rule_expression_, 0);

  while (!pending.empty()) {
   auto [parent, idx] = pending.back();
   pending.pop_back();
   RuleExpression* const child = parent->get_child_at(idx);

   switch (child->get_tag()) {
    case RuleExpression::Tag::Choice:
     if (child->get_children_size() == 1) {
      RuleExpression::UniqueHandle grandchild = child->take_child_at(0);
      parent->take_child_at(idx);
      parent->give_child_at(idx, std::move(grandchild));
      pending.emplace_back(parent, idx);
      repeat = true;
     } else {
      // Any choices in choices can be immediately flattened
      for (size_t j = 0; j < child->get_children_size(); ++j) {
       if (child->get_child_at(j)->get_tag() != RuleExpression::Tag::Choice) {
        continue;
       }

       RuleExpression::UniqueHandle grandchild = child->take_child_at(j);
       while (grandchild->get_children_size() > 0) {
        child->give_child_at(child->get_children_size(), grandchild->take_child_at(grandchild->get_children_size() - 1));
       }
       j = 0;
       repeat = true;
      }

      for (size_t j = 0; j < child->get_children_size(); ++j) {
       pending.emplace_back(child, j);
      }
     }
     break;
    case RuleExpression::Tag::Sequence:
     // Unpack any nested sequences
     for (size_t j = 0; j < child->get_children_size(); ++j) {
      RuleExpression* const grandchild = child->get_child_at(j);
      if (grandchild->get_tag() != RuleExpression::Tag::Sequence) {
       continue;
      }
      child->unpack(j);
      repeat = true;
      j = 0;
     }
     // Merge neighboring string literals
     for (size_t j = 0; j < child->get_children_size() - 1; ++j) {
      RuleExpression* const lhs = child->get_child_at(j);
      RuleExpression* const rhs = child->get_child_at(j + 1);
      if (lhs->get_tag() != RuleExpression::Tag::Literal || rhs->get_tag() != RuleExpression::Tag::Literal) {
       continue;
      }
      lhs->get_literal().insert(lhs->get_literal().end(), rhs->get_literal().begin(), rhs->get_literal().end());
      child->take_child_at(j + 1);
      repeat = true;
      j = 0;
     }
     // Remove any epsilons which are not the only child
     for (size_t i = 1; i < child->get_children_size();) {
      if (child->get_child_at(i)->get_tag() == RuleExpression::Tag::Epsilon) {
       child->take_child_at(i);
      } else {
       ++i;
      }
     }
     if (child->get_children_size() > 1 && child->get_child_at_front()->get_tag() == RuleExpression::Tag::Epsilon) {
      child->take_child_at_front();
     }
     if (child->get_children_size() == 1) {
      RuleExpression::UniqueHandle grandchild = child->take_child_at(0);
      parent->take_child_at(idx);
      parent->give_child_at(idx, std::move(grandchild));
      pending.emplace_back(parent, idx);
      repeat = true;
     } else {
      bool hasChoices = false;
      for (size_t j = 0; j < child->get_children_size(); ++j) {
       if (child->get_child_at(j)->get_tag() != RuleExpression::Tag::Choice) {
        continue;
       }
       hasChoices = true;

       RuleExpression::UniqueHandle grandchild = child->take_child_at(j);
       RuleExpression::UniqueHandle new_choice = RuleExpression::construct(RuleExpression::Tag::Choice);

       for (size_t k = 0; k < grandchild->get_children_size(); ++k) {
        RuleExpression::UniqueHandle cloned = RuleExpression::clone(*child);
        cloned->give_child_at(j, RuleExpression::clone(*grandchild->get_child_at(k)));
        new_choice->give_child_at(k, std::move(cloned));
       }

       parent->take_child_at(idx);
       parent->give_child_at(idx, std::move(new_choice));
       pending.emplace_back(parent, idx);
       repeat = true;
       break;
      }
      if (!hasChoices) {
       for (size_t j = 0; j < child->get_children_size(); ++j) {
        pending.emplace_back(child, j);
       }
      }
     }
     break;
    case RuleExpression::Tag::OneOrMore:
    case RuleExpression::Tag::NotFollowedBy:
     pending.emplace_back(child, 0);
     break;
    default:
     break;
   }
  }
 } while (repeat);
}

void flatten(Grammar& grammar_) {
 std::unordered_set<std::string> all_rule_names = grammar_.get_rule_names();
 for (std::string const& rule_name : all_rule_names) {
  Rule* rule = grammar_.get_rule(rule_name);
  if (rule == nullptr || rule->_definition == nullptr) {
   continue;
  }

  flatten_expression(rule->_definition.get());
 }
}
}  // namespace

void GrammarSimplifier::simplify(Args args_) {
 prune(args_._grammar);
 flatten(args_._grammar);
}

}  // namespace pmt::parser::grammar