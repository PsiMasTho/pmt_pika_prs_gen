// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/string_literal.hpp"
#endif
// clang-format on

#include <algorithm>

namespace pmt::base {

template <typename CHAR_TYPE_, size_t N_>
constexpr string_literal<CHAR_TYPE_, N_>::string_literal(CHAR_TYPE_ const (&str_)[N_]) {
  std::copy_n(str_, N_, _value);
}

}  // namespace pmt::base
