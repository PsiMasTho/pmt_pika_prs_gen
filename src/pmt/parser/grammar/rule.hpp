#pragma once

#include "pmt/parser/grammar/rule_expression.hpp"
#include "pmt/parser/grammar/rule_parameters.hpp"

namespace pmt::parser::grammar {

class Rule {
public:
 RuleParameters _parameters;
 RuleExpression::UniqueHandle _definition;
};

}  // namespace pmt::parser::grammar