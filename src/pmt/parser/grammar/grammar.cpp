#include "pmt/parser/grammar/grammar.hpp"
#include "parser/grammar/rule.hpp"

namespace pmt::parser::grammar {

auto Grammar::get_start_rule_name() const -> std::string const& {
 return _start_rule;
}

void Grammar::set_start_rule_name(std::string name_) {
 _start_rule = std::move(name_);
}

auto Grammar::get_rule(std::string const& name_) -> Rule* {
 auto const itr = _rules.find(name_);
 return itr != _rules.end() ? &itr->second : nullptr;
}

auto Grammar::get_rule(std::string const& name_) const -> Rule const* {
 auto const itr = _rules.find(name_);
 return itr != _rules.end() ? &itr->second : nullptr;
}

auto Grammar::get_or_create_rule(std::string const& name_) -> Rule& {
 return _rules[name_];
}

auto Grammar::get_rule_names() const -> std::unordered_set<std::string> {
 std::unordered_set<std::string> names;
 for (auto const& [name, _] : _rules) {
  names.insert(name);
 }
 return names;
}

void Grammar::remove_rule(std::string const& name_) {
 _rules.erase(name_);
}

}  // namespace pmt::parser::grammar