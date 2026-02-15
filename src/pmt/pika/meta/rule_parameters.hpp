#pragma once

#include "pmt/pika/rt/rule_parameters_base.hpp"

#include <string_view>

namespace pmt::pika::meta {

class RuleParameters : public pmt::pika::rt::RuleParametersBase {
public:
 // -$ Data $-
 std::string _display_name;  // Defaults to the rule name
 std::string _id_string;
 pmt::pika::rt::IdType _id_value = pmt::pika::rt::RuleParametersBase::ID_VALUE_DEFAULT;
 bool _merge : 1 = pmt::pika::rt::RuleParametersBase::MERGE_DEFAULT;
 bool _unpack : 1 = pmt::pika::rt::RuleParametersBase::UNPACK_DEFAULT;
 bool _hide : 1 = pmt::pika::rt::RuleParametersBase::HIDE_DEFAULT;

 // -$ Functions $-
 // --$ Lifetime $--
 virtual ~RuleParameters() = default;
 // --$ Inherited : pmt::pika::rt::RuleParametersBase $--
 auto get_display_name() const -> std::string_view override;
 auto get_id_value() const -> pmt::pika::rt::IdType override;
 auto get_merge() const -> bool override;
 auto get_unpack() const -> bool override;
 auto get_hide() const -> bool override;
};

}  // namespace pmt::pika::meta