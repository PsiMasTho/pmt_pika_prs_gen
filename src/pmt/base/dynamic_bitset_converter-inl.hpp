// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/dynamic_bitset_converter.hpp"
#endif
// clang-format on

#include <cassert>
#include <limits>

namespace pmt::base {

template <std::integral T_>
auto DynamicBitsetConverter::to_set(DynamicBitset const& bitset_) -> std::set<T_> {
  assert(bitset_.size() <= std::numeric_limits<T_>::max());

  std::set<T_> ret;
  for (T_ i = 0; i < bitset_.size(); ++i) {
    if (bitset_.get(i)) {
      ret.insert(i);
    }
  }
  return ret;
}

template <std::integral T_>
auto DynamicBitsetConverter::from_set(std::set<T_> const& set_, size_t size_) -> DynamicBitset {
  DynamicBitset ret(size_);
  for (T_ const i : set_) {
    ret.set(i, true);
  }
  return ret;
}

template <std::integral T_>
auto DynamicBitsetConverter::to_unordered_set(DynamicBitset const& bitset_) -> std::unordered_set<T_> {
  assert(bitset_.size() <= std::numeric_limits<T_>::max());

  std::unordered_set<T_> ret;
  for (T_ i = 0; i < bitset_.size(); ++i) {
    if (bitset_.get(i)) {
      ret.insert(i);
    }
  }
  return ret;
}

template <std::integral T_>
auto DynamicBitsetConverter::from_unordered_set(std::unordered_set<T_> const& set_, size_t size_) -> DynamicBitset {
  DynamicBitset ret(size_);
  for (T_ const i : set_) {
    ret.set(i, true);
  }
  return ret;
}

}  // namespace pmt::base