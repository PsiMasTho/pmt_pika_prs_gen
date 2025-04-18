#pragma once

#include <concepts>
#include <cstddef>
#include <span>
#include <utility>

namespace pmt::base {

struct IntervalIndex {
  size_t _idx : 63;
  bool _inside : 1;
};

using IntervalIndexPair = std::pair<IntervalIndex, IntervalIndex>;

template <std::integral T_>
using IntegralSpan = std::span<T_>;

template <std::integral T_>
using IntegralSpanConst = std::span<T_ const>;

template <std::integral T_>
class Interval {
  // -$ Data $-
  T_ _lower;
  T_ _upper;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  explicit Interval(T_ value_);
  Interval(T_ lower_, T_ upper_);

  // --$ Other $--
  auto get_lower() const -> T_;
  auto get_upper() const -> T_;

  // --$ Operators $--
  auto operator==(Interval const& _) const -> bool = default;
};

template <std::integral T_>
auto find_interval_index(IntegralSpanConst<T_> lowers_, IntegralSpanConst<T_> uppers_, T_ key_, size_t lskip_idx_ = 0) -> IntervalIndex;

template <std::integral T_>
auto find_interval_index_pair(IntegralSpanConst<T_> lowers_, IntegralSpanConst<T_> uppers_, Interval<T_> interval_) -> IntervalIndexPair;

}  // namespace pmt::base

#include "pmt/base/interval_container_common-inl.hpp"