#include "pmt/pika/meta/grammar.hpp"

#include <cassert>

namespace pmt::pika::meta {

auto Grammar::get_start_expression() const -> RuleExpression::UniqueHandle {
 if (_start_rule_names.size() == 1) {
  RuleExpression::UniqueHandle ret = RuleExpression::construct(pmt::pika::rt::ClauseBase::Tag::Identifier);
  ret->set_identifier(*_start_rule_names.begin());
  return ret;
 }

 RuleExpression::UniqueHandle ret = RuleExpression::construct(pmt::pika::rt::ClauseBase::Tag::Sequence);
 for (std::string const& start_rule_name : _start_rule_names) {
  RuleExpression::UniqueHandle ident = RuleExpression::construct(pmt::pika::rt::ClauseBase::Tag::Identifier);
  ident->set_identifier(start_rule_name);
  ret->give_child_at_back(std::move(ident));
 }

 return ret;
}

auto Grammar::get_start_rule_names() const -> std::unordered_set<std::string> const& {
 return _start_rule_names;
}

void Grammar::add_start_rule_name(std::string name_) {
 _start_rule_names.emplace(std::move(name_));
}

auto Grammar::get_rule_names() const -> std::unordered_set<std::string> {
 std::unordered_set<std::string> ret;
 for (auto const& [name, rule] : _rules) {
  ret.insert(name);
 }
 return ret;
}

auto Grammar::get_rule(std::string const& name_) -> Rule* {
 auto const itr = _rules.find(name_);
 if (itr != _rules.end()) {
  return &itr->second;
 }
 return nullptr;
}

auto Grammar::get_rule(std::string const& name_) const -> Rule const* {
 auto const itr = _rules.find(name_);
 if (itr != _rules.end()) {
  return &itr->second;
 }
 return nullptr;
}

void Grammar::insert_or_assign_rule(std::string const& name_, Rule rule_) {
 _id_table.insert_id(rule_._parameters._id_string);
 _rules.insert_or_assign(name_, std::move(rule_));
}

void Grammar::remove_rule(std::string const& name_) {
 _rules.erase(name_);
}

auto Grammar::get_id_table() const -> IdTable const& {
 return _id_table;
}

}  // namespace pmt::pika::meta