#include "pmt/pika/meta/prune_grammar.hpp"

#include "pmt/pika/meta/grammar.hpp"

namespace pmt::pika::meta {
using namespace pmt::pika::rt;
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
}  // namespace

void prune_grammar(Grammar& grammar_) {
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

 for (std::string const& start_rule_name : grammar_.get_start_rule_names()) {
  push_and_visit(start_rule_name);
 }

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

}  // namespace pmt::pika::meta