#pragma once

#include <concepts>
#include <cstddef>
#include <optional>
#include <span>
#include <utility>

namespace pmt::container {

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
 constexpr explicit Interval(T_ value_);
 constexpr Interval(T_ lower_, T_ upper_);

 // --$ Other $--
 constexpr auto get_lower() const -> T_;
 constexpr auto get_upper() const -> T_;

 // --$ Operators $--
 constexpr auto operator==(Interval const& _) const -> bool = default;

 // --$ Other $--
 auto hash() const -> size_t;

 // bitwise operations
 void inplace_or(Interval const& other_);
 auto clone_and(Interval const& other_) const -> std::optional<Interval>;

 // iteration
 template <std::invocable<T_> F_>
 void for_each_key(F_&& f_) const;
};

template <std::integral T_>
auto find_interval_index(IntegralSpanConst<T_> lowers_, IntegralSpanConst<T_> uppers_, T_ key_, size_t lskip_idx_ = 0) -> IntervalIndex;

template <std::integral T_>
auto find_interval_index_pair(IntegralSpanConst<T_> lowers_, IntegralSpanConst<T_> uppers_, Interval<T_> interval_) -> IntervalIndexPair;

}  // namespace pmt::container

#include "pmt/container/interval_container_common-inl.hpp"