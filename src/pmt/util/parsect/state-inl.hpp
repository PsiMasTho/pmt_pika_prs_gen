// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/util/parsect/state.hpp"
#endif
// clang-format on

#include "pmt/asserts.hpp"

#include <type_traits>

namespace pmt::util::parsect {

template <typename T_>
auto State::fetch_symbol_kind(T_ const& item_) -> Symbol::KindType {
  if constexpr (std::is_same_v<Symbol, T_>) {
    return item_._kind;
  } else if constexpr (std::is_same_v<std::vector<Symbol>, T_>) {
    return item_.front()._kind;
  } else {
    pmt::unreachable();
  }
}

auto State::symbol_kind_lt(auto const& lhs_, auto const& rhs_) -> bool {
  return fetch_symbol_kind(lhs_) < fetch_symbol_kind(rhs_);
}

}  // namespace pmt::util::parsect