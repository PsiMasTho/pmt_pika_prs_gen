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
auto DynamicBitsetConverter::from_set(std::set<T_> const& set_) -> DynamicBitset {
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
auto DynamicBitsetConverter::from_unordered_set(std::unordered_set<T_> const& set_) -> DynamicBitset {
  DynamicBitset ret(size_);
  for (T_ const i : set_) {
    ret.set(i, true);
  }
  return ret;
}

template <std::integral T_ = size_t>
auto to_interval_set(DynamicBitset const& bitset_) -> IntervalSet<T_> {
  assert(bitset_.size() <= std::numeric_limits<T_>::max());

  IntervalSet<T_> ret;
  for (T_ i = 0; i < bitset_.size(); ++i) {
    if (bitset_.get(i)) {
      T_ lower = i;
      while (i < bitset_.size() && bitset_.get(i)) {
        ++i;
      }
      T_ upper = i - 1;
      ret.insert(Interval<T_>(lower, upper));
    }
  }
  return ret;
}

template <std::integral T_ = size_t>
auto from_interval_set(IntervalSet<T_> const& interval_set_) -> DynamicBitset {
  DynamicBitset ret(size_);
  for (size_t i = 0; i < interval_set_.size(); ++i) {
    Interval<T_> const interval = interval_set_.get(i);
    for (T_ j = interval.get_lower(); j <= interval.get_upper(); ++j) {
      ret.set(j, true);
    }
  }
  return ret;
}

}  // namespace pmt::base