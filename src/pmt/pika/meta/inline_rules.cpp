#include "pmt/pika/meta/inline_rules.hpp"

#include "pmt/pika/meta/grammar.hpp"
#include "pmt/pika/meta/rule_expression.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/overloaded.hpp"

#include <string_view>
namespace pmt::pika::meta {
using namespace pmt::pika::rt;
namespace {
struct ExpressionPosition {
 RuleExpression* _parent;
 size_t _child_idx;
};

constexpr std::string_view plus_body_prefix = "__plus_body_";

auto is_plus_body_rule(std::string const& rule_name_) -> bool {
 return rule_name_.starts_with(plus_body_prefix);
}

auto get_inlineable_rules(Grammar& grammar_) -> std::unordered_set<std::string> {
 std::unordered_set<std::string> ret = grammar_.get_rule_names();

 std::vector<std::pair<RuleExpression const*, std::unordered_set<std::string>>> pending;
 std::unordered_set<std::string> visited = grammar_.get_start_rule_names();
 pending.reserve(visited.size());
 for (std::string const& start_rule_name : visited) {
  pending.emplace_back(grammar_.get_rule(start_rule_name)->_definition.get(), std::unordered_set<std::string>{start_rule_name});
 }

 while (!pending.empty()) {
  auto [expr_cur, visited_cur] = pending.back();
  pending.pop_back();

  switch (expr_cur->get_tag()) {
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::CharsetLiteral:
   case ClauseBase::Tag::NegativeLookahead: {
    for (size_t i = 0; i < expr_cur->get_children_size(); ++i) {
     pending.emplace_back(expr_cur->get_child_at(i), visited_cur);
    }
   } break;
   case ClauseBase::Tag::Identifier: {
    std::string const rule_name_cur = expr_cur->get_identifier();
    Rule const* rule = grammar_.get_rule(rule_name_cur);
    if (rule == nullptr || is_plus_body_rule(rule_name_cur) || !rule->_parameters.get_unpack() || rule->_parameters.get_merge() || rule->_parameters.get_hide()) {
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
}  // namespace

void inline_rules(Grammar& grammar_) {
 using Expression = std::variant<RuleExpression::UniqueHandle*, ExpressionPosition>;

 std::unordered_set<std::string> const unpackable_rule_names = get_inlineable_rules(grammar_);

 std::vector<std::pair<Expression, std::unordered_set<std::string>>> pending;
 pending.reserve(grammar_.get_start_rule_names().size());
 for (std::string const& start_rule_name : grammar_.get_start_rule_names()) {
  pending.emplace_back(&grammar_.get_rule(start_rule_name)->_definition, std::unordered_set<std::string>{start_rule_name});
 }

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
     case ClauseBase::Tag::NegativeLookahead: {
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
     case ClauseBase::Tag::NegativeLookahead: {
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

}  // namespace pmt::pika::meta
