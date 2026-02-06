#pragma once

#include "pmt/fw_decl.hpp"

#include <cstdint>
#include <limits>

PMT_FW_DECL_NS_CLASS(pmt::rt, Ast)

namespace pmt::meta {

class Number {
public:
 using NumberType = uint64_t;

 // -$ Data $-
private:
 NumberType _value;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit Number(pmt::rt::Ast const &ast_, uintmax_t max_value_ = std::numeric_limits<NumberType>::max());

 // --$ Other $--
 [[nodiscard]] auto get_value() const -> NumberType;
};

}  // namespace pmt::meta