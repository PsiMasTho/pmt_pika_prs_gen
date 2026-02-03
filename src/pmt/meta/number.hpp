#pragma once

#include "pmt/fw_decl.hpp"

#include <cstdint>

PMT_FW_DECL_NS_CLASS(pmt::ast, Ast)

namespace pmt::meta {

class Number {
public:
 // -$ Types / Constants $-
 using NumberType = uint64_t;

 // -$ Data $-
private:
 NumberType _value;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit Number(pmt::ast::Ast const &ast_);

 // --$ Other $--
 [[nodiscard]] auto get_value() const -> NumberType;
};

}  // namespace pmt::meta