#pragma once

#include "pmt/rt/rule_parameters_base.hpp"

#include <string_view>

namespace pmt::meta {

class RuleParameters : public pmt::rt::RuleParametersBase {
public:
 // -$ Data $-
 std::string _display_name;  // Defaults to the rule name
 std::string _id_string;
 pmt::rt::IdType _id_value = pmt::rt::RuleParametersBase::ID_VALUE_DEFAULT;
 bool _merge : 1 = pmt::rt::RuleParametersBase::MERGE_DEFAULT;
 bool _unpack : 1 = pmt::rt::RuleParametersBase::UNPACK_DEFAULT;
 bool _hide : 1 = pmt::rt::RuleParametersBase::HIDE_DEFAULT;

 // -$ Functions $-
 // --$ Inherited : pmt::rt::RuleParametersBase $--
 auto get_display_name() const -> std::string_view override;
 auto get_id_value() const -> pmt::rt::IdType override;
 auto get_merge() const -> bool override;
 auto get_unpack() const -> bool override;
 auto get_hide() const -> bool override;
};

}  // namespace pmt::meta