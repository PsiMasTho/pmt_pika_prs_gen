#pragma once

#include <cstddef>
#include <type_traits>

namespace pmt::base {

template <typename CHAR_TYPE_, size_t N_>
struct string_literal {
  constexpr string_literal(CHAR_TYPE_ const (&str_)[N_]);
  static constexpr size_t _size = N_ - 1;
  CHAR_TYPE_ _value[N_];
};

struct string_literal_type_traits {
  template <typename T_>
  struct is_string_literal : std::false_type {};

  template <typename CHAR_TYPE_, size_t N_>
  struct is_string_literal<string_literal<CHAR_TYPE_, N_>> : std::true_type {};
};

template <typename T_>
concept is_string_literal = string_literal_type_traits::is_string_literal<T_>::value;

}  // namespace pmt::base

#include "pmt/base/string_literal-inl.hpp"