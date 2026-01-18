#pragma once

#include "pmt/meta/id_table.hpp"
#include "pmt/meta/rule.hpp"

#include <unordered_map>
#include <unordered_set>

namespace pmt::meta {

class Grammar {
 // -$ Data $-
 IdTable _id_table;
 std::unordered_map<std::string, Rule> _rules;  // name -> rule
 std::string _start_rule_name;

public:
 // -$ Functions $-
 // --$ Other $--
 auto get_start_expression() const -> RuleExpression::UniqueHandle;

 auto get_start_rule_name() const -> std::string const&;
 void set_start_rule_name(std::string name_);
 auto get_rule_names() const -> std::unordered_set<std::string>;

 auto get_rule(std::string const& name_) -> Rule*;
 auto get_rule(std::string const& name_) const -> Rule const*;

 void insert_or_assign_rule(std::string const& name_, Rule rule_);
 void remove_rule(std::string const& name_);

 auto get_id_table() const -> IdTable const&;
};

}  // namespace pmt::meta