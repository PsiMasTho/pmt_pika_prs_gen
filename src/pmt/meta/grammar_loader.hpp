#pragma once

#include "pmt/meta/grammar.hpp"

#include <string>
#include <string_view>
#include <unordered_set>

namespace pmt::meta {

class GrammarLoader {
 // -$ Data $-
 std::vector<std::string_view> _inputs;
 std::unordered_set<std::string> _start_rule_names;

public:
 // -$ Functions $-
 void push_start_rule(std::string start_rule_name_);
 void push_input(std::string_view input_);
 auto load_grammar() -> Grammar;
};

}  // namespace pmt::meta