#pragma once

#include "pmt/parser/rt/rule_parameters_base.hpp"

namespace pmt::parser::grammar {

class RuleParameters : public pmt::parser::rt::RuleParametersBase {
public:
 // -$ Data $-
 std::string _display_name;
 std::string _id_string;
 GenericId::IdType _id_value = GenericId::IdUninitialized;
 bool _merge : 1 = pmt::parser::rt::RuleParametersBase::MERGE_DEFAULT;
 bool _unpack : 1 = pmt::parser::rt::RuleParametersBase::UNPACK_DEFAULT;
 bool _hide : 1 = pmt::parser::rt::RuleParametersBase::HIDE_DEFAULT;

 // -$ Functions $-
 // --$ Inherited : pmt::parser::rt::RuleParametersBase $--
 auto get_display_name() const -> std::string_view override;
 auto get_id_string() const -> std::string_view override;
 auto get_id_value() const -> GenericId::IdType override;
 auto get_merge() const -> bool override;
 auto get_unpack() const -> bool override;
 auto get_hide() const -> bool override;
};

}  // namespace pmt::parser::grammar