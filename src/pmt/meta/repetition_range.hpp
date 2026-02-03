#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/meta/number.hpp"

#include <optional>

PMT_FW_DECL_NS_CLASS(pmt::ast, Ast)

namespace pmt::meta {

class RepetitionRange {
 // -$ Data $-
 Number::NumberType _lower = 0;
 std::optional<Number::NumberType> _upper;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 RepetitionRange() = default;
 explicit RepetitionRange(pmt::ast::Ast const &ast_);

 // --$ Other $--
 [[nodiscard]] auto get_lower() const -> Number::NumberType;
 [[nodiscard]] auto get_upper() const -> std::optional<Number::NumberType>;

 void set_lower(Number::NumberType lower_);
 void set_upper(std::optional<Number::NumberType> upper_);
};

}  // namespace pmt::meta