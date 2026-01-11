#include "pmt/parser/grammar/rule_parameters.hpp"

namespace pmt::parser::grammar {

auto RuleParameters::get_display_name() const -> std::string_view {
 return _display_name;
}

auto RuleParameters::get_id_string() const -> std::string_view {
 return _id_string;
}

auto RuleParameters::get_id_value() const -> GenericId::IdType {
 return _id_value;
}

auto RuleParameters::get_merge() const -> bool {
 return _merge;
}

auto RuleParameters::get_unpack() const -> bool {
 return _unpack;
}

auto RuleParameters::get_hide() const -> bool {
 return _hide;
}

}  // namespace pmt::parser::grammar