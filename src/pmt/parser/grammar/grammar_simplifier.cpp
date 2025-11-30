#include "pmt/parser/grammar/grammar_simplifier.hpp"

#include "asserts.hpp"
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

auto get_rule_regularity_lut(Grammar const& grammar_) -> std::unordered_map<std::string, bool> {
 std::unordered_map<std::string, bool> ret;

 for (std::string const& rule_name_cur : grammar_.get_rule_names()) {
  struct StackItem {
   RuleExpression const* _expr = nullptr;
   std::unordered_set<std::string> _visited_rules;
  };

  std::vector<StackItem> pending{StackItem{._expr = grammar_.get_rule(rule_name_cur)->_definition.get(), ._visited_rules = {rule_name_cur}}};

  while (!pending.empty() && !ret.contains(rule_name_cur)) {
   auto [expr_cur, visited_rules] = pending.back();
   pending.pop_back();

   auto const mark_false = [&]() {
    for (std::string const& name : visited_rules) {
     ret.insert_or_assign(name, false);
    }
   };

   switch (expr_cur->get_tag()) {
    case ClauseBase::Tag::Sequence:
    case ClauseBase::Tag::Choice:
    case ClauseBase::Tag::Hidden:
    case ClauseBase::Tag::NotFollowedBy:
    case ClauseBase::Tag::OneOrMore: {
     for (size_t i = 0; i < expr_cur->get_children_size(); ++i) {
      pending.emplace_back(expr_cur->get_child_at(i), visited_rules);
     }
    } break;
    case ClauseBase::Tag::Identifier: {
     std::string const& rule_name_next = expr_cur->get_identifier();
     if (auto const itr = ret.find(rule_name_next); itr != ret.end()) {
      if (!itr->second) {
       mark_false();
      }
      break;
     }
     if (visited_rules.contains(rule_name_next)) {
      mark_false();
      break;
     }
     visited_rules.insert(rule_name_next);
     pending.emplace_back(grammar_.get_rule(rule_name_next)->_definition.get(), visited_rules);
    } break;
    case ClauseBase::Tag::CharsetLiteral:
    case ClauseBase::Tag::PegRegular:
    case ClauseBase::Tag::CfgRegular:
    case ClauseBase::Tag::Epsilon:
     break;
   }
  }

  ret.try_emplace(rule_name_cur, true);
 }

 return ret;
}

auto is_regular(RuleExpression const& expr_, std::unordered_map<std::string, bool> const& rule_regularity_lut_) -> bool {
 std::vector<RuleExpression const*> pending{&expr_};

 while (!pending.empty()) {
  RuleExpression const* expr_cur = pending.back();
  pending.pop_back();

  switch (expr_cur->get_tag()) {
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::Hidden:
   case ClauseBase::Tag::NotFollowedBy:
   case ClauseBase::Tag::OneOrMore: {
    for (size_t i = 0; i < expr_cur->get_children_size(); ++i) {
     pending.push_back((expr_cur->get_child_at(i)));
    }
   } break;
   case ClauseBase::Tag::Identifier: {
    if (!rule_regularity_lut_.find(expr_cur->get_identifier())->second) {
     return false;
    }
   } break;
   case ClauseBase::Tag::CharsetLiteral:
   case ClauseBase::Tag::PegRegular:
   case ClauseBase::Tag::CfgRegular:
   case ClauseBase::Tag::Epsilon:
    break;
  }
 }

 return true;
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
        child->give_child_at(child->get_children_size(), grandchild->take_child_at_front());
       }
       j = 0;
       repeat = true;
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
      while (j + 1 < child->get_children_size() && child->get_child_at(j)->get_tag() == ClauseBase::Tag::CfgRegular && child->get_child_at(j + 1)->get_tag() == ClauseBase::Tag::CfgRegular) {
       if (child->get_child_at(j)->get_child_at_front()->get_tag() != ClauseBase::Tag::Sequence) {
        RuleExpression::UniqueHandle tmp = child->get_child_at(j)->take_child_at_front();
        child->get_child_at(j)->give_child_at_front(RuleExpression::construct(ClauseBase::Tag::Sequence));
        child->get_child_at(j)->get_child_at_front()->give_child_at_back(std::move(tmp));
       }
       child->get_child_at(j)->get_child_at_front()->give_child_at_back(child->take_child_at(j + 1));
       repeat = true;
      }
     }

     // Merge neighboring string literals
     for (size_t j = 0; j < child->get_children_size() - 1; ++j) {
      RuleExpression* const lhs = child->get_child_at(j);
      RuleExpression* const rhs = child->get_child_at(j + 1);
      if (lhs->get_tag() != ClauseBase::Tag::CharsetLiteral || rhs->get_tag() != ClauseBase::Tag::CharsetLiteral || lhs->get_charset_literal().is_hidden() != rhs->get_charset_literal().is_hidden()) {
       continue;
      }

      for (size_t k = 0; k < rhs->get_charset_literal().size(); ++k) {
       lhs->get_charset_literal().push_back(rhs->get_charset_literal().get_symbol_set_at(k));
      }
      child->take_child_at(j + 1);
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
      pending.emplace_back(parent, idx, in_regular);
      repeat = true;
     } else {
      for (size_t j = 0; j < child->get_children_size(); ++j) {
       pending.emplace_back(child, j, in_regular);
      }
     }
     break;
    case ClauseBase::Tag::OneOrMore:
    case ClauseBase::Tag::NotFollowedBy:
     pending.emplace_back(child, 0, in_regular);
     break;
    case ClauseBase::Tag::Hidden: {
     switch (child->get_child_at_front()->get_tag()) {
      case ClauseBase::Tag::CharsetLiteral: {
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
      case ClauseBase::Tag::CfgRegular: {
       RuleExpression::UniqueHandle grandchild_replacement = RuleExpression::construct(child->get_child_at_front()->get_tag());
       while (child->get_child_at_front()->get_children_size() != 0) {
        grandchild_replacement->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Hidden));
        grandchild_replacement->get_child_at_back()->give_child_at_back(child->get_child_at_front()->take_child_at_front());
       }
       parent->take_child_at(idx);
       parent->give_child_at(idx, std::move(grandchild_replacement));
       pending.emplace_back(parent, idx, in_regular);
       repeat = true;
      } break;
      default:
       break;
     }
    } break;
    case ClauseBase::Tag::CfgRegular: {
     if (in_regular) {
      RuleExpression::UniqueHandle grandchild_replacement = child->take_child_at_front();
      parent->take_child_at(idx);
      parent->give_child_at(idx, std::move(grandchild_replacement));
      pending.emplace_back(parent, idx, true);
     } else {
      pending.emplace_back(child, 0, true);
     }
    } break;
    case ClauseBase::Tag::PegRegular: {
     pmt::unreachable();
    }
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

void add_peg_regular_expressions_(RuleExpression::UniqueHandle& expression_, std::unordered_map<std::string, bool> const& rule_regularity_lut_) {
 // Wrap the root expression in a dummy sequence
 {
  RuleExpression::UniqueHandle tmp = RuleExpression::construct(ClauseBase::Tag::Sequence);
  tmp->give_child_at_back(std::move(expression_));
  expression_ = std::move(tmp);
 }

 class StackItem {
 public:
  RuleExpression* _parent = nullptr;
  size_t _idx = 0;
  bool _in_regular = false;
 };

 std::vector<StackItem> pending{{._parent = expression_.get()}};

 while (!pending.empty()) {
  auto [parent_cur, idx_cur, in_regular_cur] = pending.back();
  pending.pop_back();

  RuleExpression* child_cur = parent_cur->get_child_at(idx_cur);
  bool in_regular_next = in_regular_cur;

  if (!in_regular_cur && is_regular(*child_cur, rule_regularity_lut_)) {
   RuleExpression::UniqueHandle tmp = RuleExpression::construct(ClauseBase::Tag::PegRegular);
   tmp->give_child_at_back(parent_cur->take_child_at(idx_cur));
   parent_cur->give_child_at(idx_cur, std::move(tmp));
   parent_cur = parent_cur->get_child_at(idx_cur);
   idx_cur = 0;
   child_cur = parent_cur->get_child_at(idx_cur);
   in_regular_next = true;
   in_regular_cur = true;
  }

  switch (child_cur->get_tag()) {
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::Hidden:
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::NotFollowedBy: {
    for (size_t i = 0; i < child_cur->get_children_size(); ++i) {
     pending.push_back(StackItem{._parent = child_cur, ._idx = i, ._in_regular = in_regular_next});
    }
   } break;
   case ClauseBase::Tag::PegRegular: {
    in_regular_next = true;
    if (in_regular_cur) {
     RuleExpression::UniqueHandle tmp = parent_cur->take_child_at(idx_cur);
     parent_cur->give_child_at(idx_cur, tmp->take_child_at(0));
     pending.push_back(StackItem{._parent = parent_cur, ._idx = idx_cur, ._in_regular = in_regular_next});
    } else {
     pending.push_back(StackItem{._parent = child_cur, ._idx = 0, ._in_regular = in_regular_next});
    }
   } break;
   case ClauseBase::Tag::CfgRegular:
   case ClauseBase::Tag::Identifier:
   case ClauseBase::Tag::CharsetLiteral:
   case ClauseBase::Tag::Epsilon:
    break;
  }
 }

 // Unwrap the root expression
 expression_ = expression_->take_child_at(0);
}

void add_peg_regular_expressions_in_grammar(Grammar& grammar_) {
 std::unordered_map<std::string, bool> const rule_regularity_lut = get_rule_regularity_lut(grammar_);

 for (std::string const& rule_name : grammar_.get_rule_names()) {
  Rule* const rule = grammar_.get_rule(rule_name);
  if (rule == nullptr || rule->_definition == nullptr) {
   continue;
  }

  add_peg_regular_expressions_(rule->_definition, rule_regularity_lut);
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

void GrammarSimplifier::simplify(Args args_) {
 prune(args_._grammar);
 flatten(args_._grammar);
 add_peg_regular_expressions_in_grammar(args_._grammar);
}

}  // namespace pmt::parser::grammar