// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/bitset_converter.hpp"
#endif
// clang-format on

#include <limits>

namespace pmt::base {

template <std::integral T_>
auto BitsetConverter::to_set(Bitset const& bitset_) -> std::set<T_> {
  if (bitset_.size() > std::numeric_limits<T_>::max()) {
    throw BitsetConverterError();
  }

  std::set<T_> ret;
  for (T_ i = 0; i < bitset_.size(); ++i) {
    if (bitset_.get(i)) {
      ret.insert(i);
    }
  }
  return ret;
}

template <std::integral T_>
auto BitsetConverter::from_set(std::set<T_> const& set_) -> Bitset {
  Bitset ret;
  for (T_ const i : set_) {
    if (i >= ret.size()) {
      ret.resize(i + 1);
    }
    ret.set(i, true);
  }
  return ret;
}

template <std::integral T_>
auto BitsetConverter::to_unordered_set(Bitset const& bitset_) -> std::unordered_set<T_> {
  if (bitset_.size() > std::numeric_limits<T_>::max()) {
    throw BitsetConverterError();
  }

  std::unordered_set<T_> ret;
  for (T_ i = 0; i < bitset_.size(); ++i) {
    if (bitset_.get(i)) {
      ret.insert(i);
    }
  }
  return ret;
}

template <std::integral T_>
auto BitsetConverter::from_unordered_set(std::unordered_set<T_> const& set_) -> Bitset {
  Bitset ret;

  for (T_ const i : set_) {
    if (i >= ret.size()) {
      ret.resize(i + 1);
    }
    ret.set(i, true);
  }

  return ret;
}

template <std::integral T_>
auto BitsetConverter::to_interval_set(Bitset const& bitset_) -> IntervalSet<T_> {
  if (bitset_.size() > std::numeric_limits<T_>::max()) {
    throw BitsetConverterError();
  }

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

template <std::integral T_>
auto BitsetConverter::from_interval_set(IntervalSet<T_> const& interval_set_) -> Bitset {
  Bitset ret;

  if (!interval_set_.empty()) {
    ret.resize(interval_set_.highest() + 1, false);
  }

  for (size_t i = 0; i < interval_set_.size(); ++i) {
    Interval<T_> const interval = interval_set_.get_by_index(i);
    T_ lower = interval.get_lower();
    T_ upper = interval.get_upper();

    for (T_ i = lower; i <= upper; ++i) {
      ret.set(i, true);
    }
  }

  return ret;
}

}  // namespace pmt::base