#include "pmt/meta/normalize_grammar.hpp"

#include "pmt/meta/grammar.hpp"

namespace pmt::meta {
using namespace pmt::rt;
namespace {
struct ExpressionPosition {
 RuleExpression* _parent = nullptr;
 size_t _idx;
};

struct Locals {
 std::vector<ExpressionPosition> _pending;
 bool _repeat;
};

void handle_choice(Locals& locals_, ExpressionPosition position_) {
 RuleExpression* const child = position_._parent->get_child_at(position_._idx);
 if (child->get_children_size() == 1) {
  RuleExpression::UniqueHandle grandchild = child->take_child_at_front();
  position_._parent->take_child_at(position_._idx);
  position_._parent->give_child_at(position_._idx, std::move(grandchild));
  locals_._pending.emplace_back(position_._parent, position_._idx);
  locals_._repeat = true;
 } else {
  // Any choices in choices can be immediately flattened
  for (size_t i = 0; i < child->get_children_size(); ++i) {
   if (child->get_child_at(i)->get_tag() != ClauseBase::Tag::Choice) {
    continue;
   }

   RuleExpression::UniqueHandle grandchild = child->take_child_at(i);
   for (size_t j = 0; grandchild->get_children_size() > 0; ++j) {
    child->give_child_at(i + j, grandchild->take_child_at_front());
   }
   i = 0;
   locals_._repeat = true;
  }

  for (size_t j = 0; j < child->get_children_size(); ++j) {
   locals_._pending.emplace_back(child, j);
  }
 }
}

void handle_sequence(Locals& locals_, ExpressionPosition position_) {
 RuleExpression* const child = position_._parent->get_child_at(position_._idx);
 // Unpack any nested sequences
 for (size_t i = 0; i < child->get_children_size(); ++i) {
  RuleExpression* const grandchild = child->get_child_at(i);
  if (grandchild->get_tag() != ClauseBase::Tag::Sequence) {
   continue;
  }
  child->unpack(i);
  locals_._repeat = true;
  i = 0;
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
  position_._parent->take_child_at(position_._idx);
  position_._parent->give_child_at(position_._idx, std::move(grandchild));
  locals_._pending.emplace_back(position_._parent, position_._idx);
  locals_._repeat = true;
 } else {
  for (size_t j = 0; j < child->get_children_size(); ++j) {
   locals_._pending.emplace_back(child, j);
  }
 }
}

void handle_passthrough(Locals& locals_, ExpressionPosition position_) {
 RuleExpression* const child = position_._parent->get_child_at(position_._idx);
 for (size_t j = 0; j < child->get_children_size(); ++j) {
  locals_._pending.emplace_back(child, j);
 }
}

void flatten_expression(RuleExpression::UniqueHandle& rule_expression_) {
 {
  RuleExpression::UniqueHandle tmp = RuleExpression::construct(ClauseBase::Tag::Choice);
  tmp->give_child_at_back(std::move(rule_expression_));
  rule_expression_ = std::move(tmp);
 }

 Locals locals;

 do {
  locals._repeat = false;

  for (size_t j = 0; j < rule_expression_->get_children_size(); ++j) {
   locals._pending.emplace_back(rule_expression_.get(), j);
  }

  while (!locals._pending.empty()) {
   ExpressionPosition const position = locals._pending.back();
   locals._pending.pop_back();

   switch (position._parent->get_child_at(position._idx)->get_tag()) {
    case ClauseBase::Tag::Choice:
     handle_choice(locals, position);
     break;
    case ClauseBase::Tag::Sequence:
     handle_sequence(locals, position);
     break;
    case ClauseBase::Tag::NegativeLookahead:
     handle_passthrough(locals, position);
     break;
    default:
     break;
   }
  }
 } while (locals._repeat);

 // A definition comes in as a ClauseBase::Tag::Choice, if it has one child we can just unpack it
 if (rule_expression_->get_tag() == ClauseBase::Tag::Choice && rule_expression_->get_children_size() == 1) {
  rule_expression_ = rule_expression_->take_child_at_back();
 }
}
}  // namespace

void normalize_grammar(Grammar& grammar_) {
 for (std::string const& rule_name : grammar_.get_rule_names()) {
  Rule* const rule = grammar_.get_rule(rule_name);
  assert(rule != nullptr && rule->_definition != nullptr);
  flatten_expression(rule->_definition);
 }
}

}  // namespace pmt::meta