// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/container/interval_container_common.hpp"
#endif
// clang-format on

#include "pmt/hash.hpp"

#include <algorithm>
#include <cassert>
#include <functional>

namespace pmt::container {

template <std::integral T_>
constexpr Interval<T_>::Interval(T_ value_)
 : Interval(value_, value_) {
}

template <std::integral T_>
constexpr Interval<T_>::Interval(T_ lower_, T_ upper_)
 : _lower(lower_)
 , _upper(upper_) {
 assert(lower_ <= upper_);
}

template <std::integral T_>
constexpr auto Interval<T_>::get_lower() const -> T_ {
 return _lower;
}

template <std::integral T_>
constexpr auto Interval<T_>::get_upper() const -> T_ {
 return _upper;
}

template <std::integral T_>
auto Interval<T_>::hash() const -> size_t {
 size_t seed = Hash::Phi64;
 Hash::combine(get_lower(), seed);
 Hash::combine(get_upper(), seed);
 return seed;
}

template <std::integral T_>
void Interval<T_>::inplace_or(Interval const& other_) {
 _lower = std::min(_lower, other_.get_lower());
 _upper = std::max(_upper, other_.get_upper());
}

template <std::integral T_>
auto Interval<T_>::clone_and(Interval const& other_) const -> std::optional<Interval> {
 Interval const* higher = _lower < other_.get_lower() ? &other_ : this;
 Interval const* lower = (higher == this) ? &other_ : this;

 if (higher->get_lower() > lower->get_upper()) {
  return std::nullopt;
 }

 return Interval{std::max(higher->get_lower(), lower->get_lower()), std::min(higher->get_upper(), lower->get_upper())};
}

template <std::integral T_>
template <std::invocable<T_> F_>
void Interval<T_>::for_each_key(F_&& f_) const {
 T_ key = get_lower();
 while (true) {
  std::invoke(std::forward<F_>(f_), key);
  if (key == get_upper()) {
   break;
  }
  ++key;
 }
}

template <std::integral T_>
auto find_interval_index(IntegralSpanConst<T_> lowers_, IntegralSpanConst<T_> uppers_, T_ key_, size_t lskip_idx_) -> IntervalIndex {
 assert(lowers_.size() == uppers_.size());

 lskip_idx_ = std::min(lowers_.size(), lskip_idx_);

 size_t const idx = [&]() {
  auto const itr = std::lower_bound(lowers_.begin() + lskip_idx_, lowers_.end(), key_);
  return std::distance(lowers_.begin(), itr);
 }();

 if (idx == lowers_.size()) {
  if (idx == 0 || key_ > uppers_[idx - 1]) {
   return IntervalIndex{._idx = idx, ._inside = false};
  }
  return IntervalIndex{._idx = idx - 1, ._inside = true};
 }

 if (key_ < lowers_[idx]) {
  if (idx == 0) {
   return IntervalIndex{._idx = 0, ._inside = false};
  }
  if (key_ <= uppers_[idx - 1]) {
   return IntervalIndex{._idx = idx - 1, ._inside = true};
  }
  return IntervalIndex{._idx = idx, ._inside = false};
 }

 return IntervalIndex{._idx = idx, ._inside = true};
}

template <std::integral T_>
auto find_interval_index_pair(IntegralSpanConst<T_> lowers_, IntegralSpanConst<T_> uppers_, Interval<T_> interval_) -> IntervalIndexPair {
 IntervalIndexPair ret;
 ret.first = find_interval_index(lowers_, uppers_, interval_.get_lower());
 ret.second = find_interval_index(lowers_, uppers_, interval_.get_upper(), ret.first._idx);
 return ret;
}

}  // namespace pmt::container
