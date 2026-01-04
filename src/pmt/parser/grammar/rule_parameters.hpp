#pragma once

#include "pmt/parser/rt/rule_parameters_view.hpp"

namespace pmt::parser::grammar {

class RuleParameters {
public:
 std::string _display_name;
 std::string _id_string;
 bool _merge : 1 = pmt::parser::rt::RuleParametersView::MERGE_DEFAULT;
 bool _unpack : 1 = pmt::parser::rt::RuleParametersView::UNPACK_DEFAULT;
 bool _hide : 1 = pmt::parser::rt::RuleParametersView::HIDE_DEFAULT;
};

}  // namespace pmt::parser::grammar