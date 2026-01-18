#include "pmt/meta/grammar.hpp"

namespace pmt::meta {

auto Grammar::get_start_expression() const -> RuleExpression::UniqueHandle {
 RuleExpression::UniqueHandle ret = RuleExpression::construct(pmt::rt::ClauseBase::Tag::Identifier);
 ret->set_identifier(_start_rule_name);
 return ret;
}

auto Grammar::get_start_rule_name() const -> std::string const& {
 return _start_rule_name;
}

void Grammar::set_start_rule_name(std::string name_) {
 _start_rule_name = std::move(name_);
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

}  // namespace pmt::meta