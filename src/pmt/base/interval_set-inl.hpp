// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/interval_set.hpp"
#endif
// clang-format on

#include "pmt/base/algo.hpp"
#include "pmt/base/hash.hpp"

#include <cassert>

namespace pmt::base {

template <std::integral KEY_>
IntervalSet<KEY_>::IntervalSet(Interval<KEY_> interval_)
: IntervalSet() {
  insert(interval_);
}

template <std::integral KEY_>
IntervalSet<KEY_>::IntervalSet(IntervalSet const& other_)
 : _intervals(nullptr)
 , _size(other_._size)
 , _capacity_idx(0) {
  reserve(other_._size);
  std::copy(other_.get_lowers().begin(), other_.get_lowers().end(), get_lowers().begin());
  std::copy(other_.get_uppers().begin(), other_.get_uppers().end(), get_uppers().begin());
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::operator=(IntervalSet const& other_) -> IntervalSet& {
  if (this == &other_) {
    return *this;
  }

  IntervalSet tmp(other_);
  _intervals = std::move(tmp._intervals);
  _size = tmp._size;
  _capacity_idx = tmp._capacity_idx;
  return *this;
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::operator==(IntervalSet const& other_) const -> bool {
  return std::ranges::equal(get_lowers(), other_.get_lowers()) && std::ranges::equal(get_uppers(), other_.get_uppers());
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::operator!=(IntervalSet const& other_) const -> bool {
  return !(*this == other_);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::hash() const -> size_t {
  uint64_t seed = Hash::Phi64;

  for (KEY_ const& lower : get_lowers()) {
    Hash::combine(lower, seed);
  }

  for (KEY_ const& upper : get_uppers()) {
    Hash::combine(upper, seed);
  }

  return seed;
}

template <std::integral KEY_>
void IntervalSet<KEY_>::insert(Interval<KEY_> interval_) {
  IntervalIndexPair indices = find_and_expand_interval_indices(interval_);

  int net = 1;

  if (indices.first._idx != indices.second._idx || indices.first._inside || indices.second._inside) {
    indices.second._idx -= (indices.second._inside || indices.second._idx == indices.first._idx) ? 0 : 1;
    net = -(indices.second._idx - indices.first._idx);
  } else if (size() + net > capacity()) {
    reserve(size() + net);
  }

  IntegralSpan<KEY_> const lowers = get_lowers();
  IntegralSpan<KEY_> const uppers = get_uppers();
  KEY_ const l = (net == 1) ? interval_.get_lower() : std::min(interval_.get_lower(), lowers[indices.first._idx]);
  KEY_ const u = (net == 1) ? interval_.get_upper() : std::max(interval_.get_upper(), uppers[indices.second._idx]);
  size_t const k = indices.first._idx + (net == 1 ? 0 : 1 - net);
  size_t const shift = size() - (net == 1 ? indices.first._idx : indices.second._idx + 1);

  move_bidir(lowers.begin() + k, lowers.begin() + k + shift, lowers.begin() + indices.first._idx + 1);
  move_bidir(uppers.begin() + k, uppers.begin() + k + shift, uppers.begin() + indices.first._idx + 1);

  lowers[indices.first._idx] = l;
  uppers[indices.first._idx] = u;

  _size += net;
}

template <std::integral KEY_>
void IntervalSet<KEY_>::erase(Interval<KEY_> interval_) {
  if (size() == 0) {
    return;
  }

  IntegralSpan<KEY_> const lowers = get_lowers();
  IntegralSpan<KEY_> const uppers = get_uppers();

  IntervalIndexPair indices = find_interval_index_pair<KEY_>(get_lowers(), get_uppers(), interval_);

  // Adjust if the indices are on the edges
  if (indices.first._inside && lowers[indices.first._idx] == interval_.get_lower()) {
    indices.first._inside = false;
  }

  if (indices.second._inside && indices.second._idx != size() && uppers[indices.second._idx] == interval_.get_upper()) {
    ++indices.second._idx;
    indices.second._inside = false;
  }

  if (indices.first._idx == indices.second._idx) {
    // Three possibilities:
    // a) both are outside   -> do nothing
    // b) only rhs is inside -> adjust lower bound
    // c) Both are inside    -> split the interval
    if (!indices.first._inside && indices.second._inside) {  // b
      lowers[indices.second._idx] = interval_.get_upper() + 1;
    } else if (indices.first._inside && indices.second._inside) {  // c
      Interval<KEY_> const interval(interval_.get_upper() + 1, uppers[indices.first._idx]);
      uppers[indices.first._idx] = interval_.get_lower() - 1;
      insert(interval);
    }
  } else {
    uppers[indices.first._idx] = indices.first._inside ? interval_.get_lower() - 1 : uppers[indices.first._idx];
    lowers[indices.second._idx] = indices.second._inside ? interval_.get_upper() + 1 : lowers[indices.second._idx];

    /* erase the intervals in between */
    size_t const lhs_adjusted = indices.first._inside ? indices.first._idx + 1 : indices.first._idx;
    size_t const between = indices.second._idx - lhs_adjusted;

    if (between != 0) {
      size_t const remaining = size() - indices.second._idx;
      std::move(lowers.begin() + indices.second._idx, lowers.begin() + indices.second._idx + remaining, lowers.begin() + lhs_adjusted);
      std::move(uppers.begin() + indices.second._idx, uppers.begin() + indices.second._idx + remaining, uppers.begin() + lhs_adjusted);
      _size -= between;
    }
  }
}

template <std::integral KEY_>
void IntervalSet<KEY_>::clear() {
  _size = 0;
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::contains(KEY_ key_) const -> bool {
  return find_interval_index<KEY_>(get_lowers(), get_uppers(), key_)._inside;
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::get_by_index(size_t index_) const -> Interval<KEY_> {
 assert(index_ < size());
 return Interval<KEY_>(get_lowers()[index_], get_uppers()[index_]);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::size() const -> size_t {
  return _size;
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::capacity() const -> size_t {
  return (_intervals == nullptr) ? 0 : AmortizedGrowth::idx_to_size(_capacity_idx);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::empty() const -> bool {
  return size() == 0;
}

template <std::integral KEY_>
void IntervalSet<KEY_>::reserve(size_t new_capacity_) {
  new_capacity_ = std::max(size(), new_capacity_);

  size_t const new_capacity_idx = AmortizedGrowth::size_to_idx(new_capacity_, InitialCapacityIdx);

  if (new_capacity_idx == _capacity_idx) {
    return;
  }

  size_t const old_capacity = capacity();
  new_capacity_ = AmortizedGrowth::idx_to_size(new_capacity_idx);
  _capacity_idx = new_capacity_idx;
  realloc_unique_ptr(_intervals, old_capacity * 2, new_capacity_ * 2);

  /* Shift up the upper bounds */
  std::move(_intervals.get() + old_capacity, _intervals.get() + old_capacity + _size, _intervals.get() + new_capacity_);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::popcnt() const -> size_t {
  size_t count = 0;
  for (size_t i = 0; i < size(); ++i) {
    count += get_uppers()[i] - get_lowers()[i] + 1;
  }
  return count;
}

template <std::integral KEY_>
void IntervalSet<KEY_>::inplace_or(IntervalSet const& other_) {
  for (size_t i = 0; i < other_.size(); ++i) {
    insert(other_.get_by_index(i));
  }
}

template <std::integral KEY_>
void IntervalSet<KEY_>::inplace_asymmetric_difference(IntervalSet const& other_) {
  for (size_t i = 0; i < other_.size(); ++i) {
    erase(other_.get_by_index(i));
  }
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::clone_and(IntervalSet const& other_) const -> IntervalSet {
  // Make sure lhs is smaller or equal
  IntervalSet const& lhs = (size() <= other_.size()) ? *this : other_;
  IntervalSet const& rhs = (this == &lhs) ? other_ : *this;

  IntervalSet ret;

  IntegralSpanConst<KEY_> const rhs_lowers = rhs.get_lowers();
  IntegralSpanConst<KEY_> const rhs_uppers = rhs.get_uppers();

  size_t i = 0;
  for (size_t j = 0; j < lhs.size(); ++j) {
    Interval<KEY_> const lhs_interval = lhs.get_by_index(j);

    /* Search in rhs from where we left off */
    IntervalIndex const rhs_index = find_interval_index<KEY_>(rhs_lowers, rhs_uppers, lhs_interval.get_lower(), i);
    i = rhs_index._idx;

    while (i < rhs.size()) {
      Interval<KEY_> rhs_interval = rhs.get_by_index(i);

      if (rhs_interval.get_lower() > lhs_interval.get_upper()) {
        /* Step back once in case the previous i would have overlapped with the next j */
        i = (i == 0) ? 0 : i - 1;
        break;
      }

      ret.insert(Interval<KEY_>(std::max(lhs_interval.get_lower(), rhs_interval.get_lower()), std::min(lhs_interval.get_upper(), rhs_interval.get_upper())));
      ++i;
    }
  }

  return ret;
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::clone_asymmetric_difference(IntervalSet const& other_) const -> IntervalSet {
  IntervalSet ret = *this;
  ret.inplace_asymmetric_difference(other_);
  return ret;
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::lowest() const -> KEY_ {
  assert(!empty());
  return get_lowers()[0];
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::highest() const -> KEY_ {
  assert(!empty());
  return get_uppers()[size() - 1];
}

template <std::integral KEY_>
template <std::invocable<Interval<KEY_>> F_>
void IntervalSet<KEY_>::for_each_interval(F_&& f_) const {
  for (size_t i = 0; i < size(); ++i) {
    std::invoke(std::forward<F_>(f_), get_by_index(i));
  }
}

template <std::integral KEY_>
template <std::invocable<KEY_> F_>
void IntervalSet<KEY_>::for_each_key(F_&& f_) const {
  for_each_interval([&f_](Interval<KEY_> const& interval_) { interval_.for_each_key(std::forward<F_>(f_)); });
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::get_lowers() const -> IntegralSpanConst<KEY_> {
  return IntegralSpanConst<KEY_>(_intervals.get(), _size);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::get_lowers() -> IntegralSpan<KEY_> {
  return IntegralSpan<KEY_>(_intervals.get(), _size);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::get_uppers() const -> IntegralSpanConst<KEY_> {
  return IntegralSpanConst<KEY_>(_intervals.get() + capacity(), _size);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::get_uppers() -> IntegralSpan<KEY_> {
  return IntegralSpan<KEY_>(_intervals.get() + capacity(), _size);
}

template <std::integral KEY_>
auto IntervalSet<KEY_>::find_and_expand_interval_indices(Interval<KEY_> interval_) -> IntervalIndexPair {
  IntegralSpan<KEY_> const lowers = get_lowers();
  IntegralSpan<KEY_> const uppers = get_uppers();
  IntervalIndexPair indices = find_interval_index_pair<KEY_>(lowers, uppers, interval_);

  if (!indices.first._inside && indices.first._idx != 0) {
    if (uppers[indices.first._idx - 1] == interval_.get_lower() - 1) {
      --indices.first._idx;
      indices.first._inside = true;
      ++uppers[indices.first._idx];
    }
  }

  if (!indices.second._inside && indices.second._idx != lowers.size()) {
    if (lowers[indices.second._idx] == interval_.get_upper() + 1) {
      indices.second._inside = true;
      --lowers[indices.second._idx];
    }
  }

  return indices;
}

}  // namespace pmt::base