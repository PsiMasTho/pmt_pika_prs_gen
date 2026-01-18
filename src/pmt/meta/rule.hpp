#pragma once

#include "pmt/meta/rule_expression.hpp"
#include "pmt/meta/rule_parameters.hpp"

namespace pmt::meta {

class Rule {
public:
 RuleParameters _parameters;
 RuleExpression::UniqueHandle _definition;
};

}  // namespace pmt::meta