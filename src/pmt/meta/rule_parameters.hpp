#pragma once

#include "pmt/ast/id.hpp"
#include "pmt/rt/rule_parameters_base.hpp"

namespace pmt::meta {

class RuleParameters : public pmt::rt::RuleParametersBase {
public:
 // -$ Data $-
 std::string _display_name;
 std::string _id_string;
 pmt::ast::IdType _id_value = pmt::ast::ReservedIds::IdUninitialized;
 bool _merge : 1 = pmt::rt::RuleParametersBase::MERGE_DEFAULT;
 bool _unpack : 1 = pmt::rt::RuleParametersBase::UNPACK_DEFAULT;
 bool _hide : 1 = pmt::rt::RuleParametersBase::HIDE_DEFAULT;

 // -$ Functions $-
 // --$ Inherited : pmt::rt::RuleParametersBase $--
 auto get_display_name() const -> std::string_view override;
 auto get_id_string() const -> std::string_view override;
 auto get_id_value() const -> pmt::ast::IdType override;
 auto get_merge() const -> bool override;
 auto get_unpack() const -> bool override;
 auto get_hide() const -> bool override;
};

}  // namespace pmt::meta