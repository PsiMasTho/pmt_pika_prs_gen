#pragma once

#include "pmt/parser/grammar/rule.hpp"

#include <unordered_map>
#include <unordered_set>

namespace pmt::parser::grammar {

class Grammar {
 // -$ Data $-
 std::unordered_map<std::string, Rule> _rules;
 std::string _start_rule;

public:
 // -$ Functions $-
 // --$ Other $--
 auto get_start_rule_name() const -> std::string const&;
 void set_start_rule_name(std::string name_);

 auto get_rule(std::string const& name_) -> Rule*;
 auto get_rule(std::string const& name_) const -> Rule const*;

 auto get_or_create_rule(std::string const& name_) -> Rule&;

 auto get_rule_names() const -> std::unordered_set<std::string>;

 void remove_rule(std::string const& name_);
};

}  // namespace pmt::parser::grammar