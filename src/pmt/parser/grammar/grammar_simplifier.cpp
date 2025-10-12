#include "pmt/parser/grammar/grammar_simplifier.hpp"

#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/grammar/charset_literal.hpp"
#include "pmt/parser/grammar/grammar.hpp"
#include "pmt/parser/grammar/rule.hpp"

#include <unordered_set>

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

void flatten_expression(RuleExpression::UniqueHandle& rule_expression_) {
 class StackItem {
 public:
  RuleExpression* _parent = nullptr;
  size_t _idx;
  bool _in_regular = false;
 };

 bool repeat = false;

 do {
  repeat = false;
  std::vector<StackItem> pending;

  for (size_t j = 0; j < rule_expression_->get_children_size(); ++j) {
   pending.emplace_back(rule_expression_.get(), j);
  }

  while (!pending.empty()) {
   auto [parent, idx, in_regular] = pending.back();
   pending.pop_back();
   RuleExpression* const child = parent->get_child_at(idx);

   switch (child->get_tag()) {
    case ClauseBase::Tag::Choice:
     if (child->get_children_size() == 1) {
      RuleExpression::UniqueHandle grandchild = child->take_child_at_front();
      parent->take_child_at(idx);
      parent->give_child_at(idx, std::move(grandchild));
      pending.emplace_back(parent, idx, in_regular);
      repeat = true;
     } else {
      // Any choices in choices can be immediately flattened
      for (size_t j = 0; j < child->get_children_size(); ++j) {
       if (child->get_child_at(j)->get_tag() != ClauseBase::Tag::Choice) {
        continue;
       }

       RuleExpression::UniqueHandle grandchild = child->take_child_at(j);
       while (grandchild->get_children_size() > 0) {
        child->give_child_at(child->get_children_size(), grandchild->take_child_at(grandchild->get_children_size() - 1));
       }
       j = 0;
       repeat = true;
      }

      // Any neighboring Regular can be merged and have the choice moved inside
      for (size_t j = 0; j + 1 < child->get_children_size(); ++j) {
       while (j + 1 < child->get_children_size() && child->get_child_at(j)->get_tag() == ClauseBase::Tag::Regular && child->get_child_at(j + 1)->get_tag() == ClauseBase::Tag::Regular) {
        if (child->get_child_at(j)->get_child_at_front()->get_tag() != ClauseBase::Tag::Choice) {
         RuleExpression::UniqueHandle tmp = child->get_child_at(j)->take_child_at_front();
         child->get_child_at(j)->give_child_at_front(RuleExpression::construct(ClauseBase::Tag::Choice));
         child->get_child_at(j)->get_child_at_front()->give_child_at_back(std::move(tmp));
        }
        child->get_child_at(j)->get_child_at_front()->give_child_at_back(child->take_child_at(j + 1));
        repeat = true;
       }
      }

      for (size_t j = 0; j < child->get_children_size(); ++j) {
       pending.emplace_back(child, j, in_regular);
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
     // Any neighboring Regular can be merged and have the sequence moved inside
     for (size_t j = 0; j + 1 < child->get_children_size(); ++j) {
      while (j + 1 < child->get_children_size() && child->get_child_at(j)->get_tag() == ClauseBase::Tag::Regular && child->get_child_at(j + 1)->get_tag() == ClauseBase::Tag::Regular) {
       if (child->get_child_at(j)->get_child_at_front()->get_tag() != ClauseBase::Tag::Sequence) {
        RuleExpression::UniqueHandle tmp = child->get_child_at(j)->take_child_at_front();
        child->get_child_at(j)->give_child_at_front(RuleExpression::construct(ClauseBase::Tag::Sequence));
        child->get_child_at(j)->get_child_at_front()->give_child_at_back(std::move(tmp));
       }
       child->get_child_at(j)->get_child_at_front()->give_child_at_back(child->take_child_at(j + 1));
       repeat = true;
      }
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
      pending.emplace_back(parent, idx, in_regular);
      repeat = true;
     } else {
      bool hasChoices = false;
      for (size_t j = 0; j < child->get_children_size(); ++j) {
       if (child->get_child_at(j)->get_tag() != ClauseBase::Tag::Choice) {
        continue;
       }
       hasChoices = true;

       RuleExpression::UniqueHandle grandchild = child->take_child_at(j);
       RuleExpression::UniqueHandle new_choice = RuleExpression::construct(ClauseBase::Tag::Choice);

       for (size_t k = 0; k < grandchild->get_children_size(); ++k) {
        RuleExpression::UniqueHandle cloned = RuleExpression::clone(*child);
        cloned->give_child_at(j, RuleExpression::clone(*grandchild->get_child_at(k)));
        new_choice->give_child_at(k, std::move(cloned));
       }

       parent->take_child_at(idx);
       parent->give_child_at(idx, std::move(new_choice));
       pending.emplace_back(parent, idx, in_regular);
       repeat = true;
       break;
      }
      if (!hasChoices) {
       for (size_t j = 0; j < child->get_children_size(); ++j) {
        pending.emplace_back(child, j, in_regular);
       }
      }
     }
     break;
    case ClauseBase::Tag::OneOrMore:
    case ClauseBase::Tag::NotFollowedBy:
     pending.emplace_back(child, 0, in_regular);
     break;
    case ClauseBase::Tag::Hidden: {
     switch (child->get_child_at_front()->get_tag()) {
      case ClauseBase::Tag::Literal: {
       RuleExpression::UniqueHandle grandchild_replacement = child->take_child_at_front();
       grandchild_replacement->get_charset_literal().set_hidden(true);
       parent->take_child_at(idx);
       parent->give_child_at(idx, std::move(grandchild_replacement));
       pending.emplace_back(parent, idx, in_regular);
      } break;
      case ClauseBase::Tag::NotFollowedBy:
      case ClauseBase::Tag::OneOrMore:
      case ClauseBase::Tag::Sequence:
      case ClauseBase::Tag::Choice:
      case ClauseBase::Tag::Regular: {
       RuleExpression::UniqueHandle grandchild_replacement = RuleExpression::construct(child->get_child_at_front()->get_tag());
       while (child->get_child_at_front()->get_children_size() != 0) {
        grandchild_replacement->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Hidden));
        grandchild_replacement->get_child_at_back()->give_child_at_back(child->get_child_at_front()->take_child_at_front());
       }
       parent->take_child_at(idx);
       parent->give_child_at(idx, std::move(grandchild_replacement));
       pending.emplace_back(parent, idx, in_regular);
       repeat = true;
      }
       repeat = true;
       break;
      default:
       break;
     }
    } break;
    case ClauseBase::Tag::Regular: {
     if (in_regular) {
      RuleExpression::UniqueHandle grandchild_replacement = child->take_child_at_front();
      parent->take_child_at(idx);
      parent->give_child_at(idx, std::move(grandchild_replacement));
      pending.emplace_back(parent, idx, true);
     } else {
      pending.emplace_back(child, 0, true);
     }
    } break;
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
  Rule* rule = grammar_.get_rule(rule_name);
  if (rule == nullptr || rule->_definition == nullptr) {
   continue;
  }

  flatten_expression(rule->_definition);
 }
}
}  // namespace

void GrammarSimplifier::simplify(Args args_) {
 prune(args_._grammar);
 flatten(args_._grammar);
}

}  // namespace pmt::parser::grammar