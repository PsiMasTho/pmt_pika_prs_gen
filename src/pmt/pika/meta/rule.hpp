#pragma once

#include "pmt/pika/meta/rule_expression.hpp"
#include "pmt/pika/meta/rule_parameters.hpp"

namespace pmt::pika::meta {

class Rule {
public:
 RuleParameters _parameters;
 RuleExpression::UniqueHandle _definition;
};

}  // namespace pmt::pika::meta