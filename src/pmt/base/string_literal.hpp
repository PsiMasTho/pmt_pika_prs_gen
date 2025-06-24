#pragma once

#include <cstddef>
#include <type_traits>

namespace pmt::base {

template <typename CHAR_TYPE_, size_t N_>
struct StringLiteral {
 constexpr StringLiteral(CHAR_TYPE_ const (&str_)[N_]);
 static constexpr size_t _size = N_ - 1;
 CHAR_TYPE_ _value[N_];
};

struct StringLiteralTraits {
 template <typename T_>
 struct IsStringLiteral : std::false_type {};

 template <typename CHAR_TYPE_, size_t N_>
 struct IsStringLiteral<StringLiteral<CHAR_TYPE_, N_>> : std::true_type {};
};

template <typename T_>
concept IsStringLiteral = StringLiteralTraits::IsStringLiteral<T_>::value;

}  // namespace pmt::base

#include "pmt/base/string_literal-inl.hpp"