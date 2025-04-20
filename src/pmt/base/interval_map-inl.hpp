// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/interval_map.hpp"
#endif
// clang-format on

#include "pmt/base/algo.hpp"

namespace pmt::base {
template <std::integral KEY_, typename VALUE_>
IntervalMap<KEY_, VALUE_>::IntervalMap(IntervalMap const& other_)
 : _intervals(nullptr)
 , _values(nullptr)
 , _size(other_._size)
 , _capacity_idx(0) {
  reserve(other_._size);
  std::copy(other_._intervals.get(), other_._intervals.get(), _intervals.get());
  for (size_t i = 0; i < _size; ++i) {
    new (_values + i) VALUE_(other_._values[i]);
  }
}

template <std::integral KEY_, typename VALUE_>
IntervalMap<KEY_, VALUE_>::IntervalMap(IntervalMap&& other_) noexcept
 : _intervals(std::move(other_._intervals))
 , _values(other_._values)
 , _size(other_._size)
 , _capacity_idx(other_._capacity_idx) {
  other_._size = 0;
  other_._capacity_idx = 0;
  other_._values = nullptr;
}

template <std::integral KEY_, typename VALUE_>
IntervalMap<KEY_, VALUE_>::~IntervalMap() {
  clear();
  ::operator delete[](_values);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::operator=(IntervalMap const& other_) -> IntervalMap& {
  if (this == &other_) {
    return *this;
  }

  IntervalMap tmp(other_);
  _intervals = std::move(tmp._intervals);
  _values = std::move(tmp._values);
  _size = tmp._size;
  _capacity_idx = tmp._capacity_idx;
  return *this;
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::operator==(IntervalMap const& other_) const -> bool {
  return std::ranges::equal(get_lowers(), other_.get_lowers()) && std::ranges::equal(get_uppers(), other_.get_uppers()) && std::ranges::equal(_values, _values + _size, other_._values);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::operator!=(IntervalMap const& other_) const -> bool {
  return !(*this == other_);
}

template <std::integral KEY_, typename VALUE_>
void IntervalMap<KEY_, VALUE_>::insert(Interval<KEY_> interval_, VALUE_ value_) {
  IntervalIndexPair indices = find_and_expand_interval_indices(interval_, value_);

  bool const split_left = indices.first._inside && _values[indices.first._idx] != value_;
  bool const split_right = indices.second._inside && _values[indices.second._idx] != value_;
  bool const same_index = indices.first._idx == indices.second._idx;

  int net = 1;

  if (split_left && split_right && same_index) {
    if (interval_.get_lower() != get_lowers()[indices.first._idx] && interval_.get_upper() != get_uppers()[indices.second._idx]) {
      // range inserted in the middle of an existing one
      net = 2;
    } else if (interval_.get_lower() == get_lowers()[indices.first._idx] && interval_.get_upper() == get_uppers()[indices.second._idx]) {
      // existing range overwritten exactly
      net = 0;
    } else {
      // one side of an existing range is overwritten
      net = 1;
      indices.first._inside = false;
      indices.second._inside = false;
      if (interval_.get_lower() == get_lowers()[indices.first._idx]) {
        // the left side is overwritten
        get_lowers()[indices.first._idx] = interval_.get_upper() + 1;
      } else {
        // the right side is overwritten
        get_uppers()[indices.second._idx] = interval_.get_lower() - 1;
        ++indices.first._idx;
        ++indices.second._idx;
      }
    }
  } else {
    if (split_left) {
      // Nothing needs to happen otherwise because then the whole interval can be overwritten
      if (get_lowers()[indices.first._idx] != interval_.get_lower()) {
        get_uppers()[indices.first._idx] = interval_.get_lower() - 1;
        indices.first._inside = false;
        ++indices.first._idx;
      }
    }
    if (split_right) {
      // Nothing needs to happen otherwise because then the whole interval can be overwritten
      if (get_uppers()[indices.second._idx] != interval_.get_upper()) {
        get_lowers()[indices.second._idx] = interval_.get_upper() + 1;
        indices.second._inside = false;
      }
    }

    if (indices.first._idx != indices.second._idx || indices.first._inside || indices.second._inside) {
      indices.second._idx -= (indices.second._inside || indices.second._idx == indices.first._idx) ? 0 : 1;
      net = -(indices.second._idx - indices.first._idx);
    }
  }

  if (net > 0) {
    if (size() + net > capacity()) {
      reserve(size() + net);
    }

    size_t const remaining = size() - indices.first._idx;
    move_bidir(get_lowers().begin() + indices.first._idx, get_lowers().begin() + indices.first._idx + remaining, get_lowers().begin() + indices.first._idx + net);
    move_bidir(get_uppers().begin() + indices.first._idx, get_uppers().begin() + indices.first._idx + remaining, get_uppers().begin() + indices.first._idx + net);
    std::uninitialized_default_construct(_values + _size, _values + _size + net);
    move_bidir(_values + indices.first._idx, _values + indices.first._idx + remaining, _values + indices.first._idx + net);

    if (net == 2) {
      get_lowers()[indices.first._idx] = get_lowers()[indices.first._idx + 2];
      get_uppers()[indices.first._idx] = interval_.get_lower() - 1;
      get_lowers()[indices.first._idx + 1] = interval_.get_lower();
      get_uppers()[indices.first._idx + 1] = interval_.get_upper();
      get_lowers()[indices.first._idx + 2] = interval_.get_upper() + 1;

      _values[indices.first._idx + 1] = std::move(value_);
      _values[indices.first._idx] = _values[indices.first._idx + 2];  // copy
    } else {
      get_lowers()[indices.first._idx] = interval_.get_lower();
      get_uppers()[indices.first._idx] = interval_.get_upper();
      _values[indices.first._idx] = std::move(value_);
    }
  } else {
    get_lowers()[indices.first._idx] = std::min(interval_.get_lower(), get_lowers()[indices.first._idx]);
    get_uppers()[indices.first._idx] = std::max(interval_.get_upper(), get_uppers()[indices.second._idx]);

    _values[indices.first._idx] = std::move(value_);

    size_t const shift = size() - (indices.second._idx + 1);

    move_bidir(get_lowers().begin() + indices.first._idx + (1 - net), get_lowers().begin() + indices.first._idx + (1 - net) + shift, get_lowers().begin() + indices.first._idx + 1);
    move_bidir(get_uppers().begin() + indices.first._idx + (1 - net), get_uppers().begin() + indices.first._idx + (1 - net) + shift, get_uppers().begin() + indices.first._idx + 1);
    move_bidir(_values + indices.first._idx + (1 - net), _values + indices.first._idx + (1 - net) + shift, _values + indices.first._idx + 1);

    // Destroy any trailing values
    std::destroy(_values + _size + net, _values + _size);
  }

  _size += net;
}

template <std::integral KEY_, typename VALUE_>
void IntervalMap<KEY_, VALUE_>::erase(Interval<KEY_> interval_) {
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
      insert(interval, _values[indices.first._idx]);
    }
  } else {
    uppers[indices.first._idx] = indices.first._inside ? interval_.get_lower() - 1 : uppers[indices.first._idx];
    lowers[indices.second._idx] = indices.second._inside ? interval_.get_upper() + 1 : lowers[indices.second._idx];

    /* erase the intervals in between */
    size_t const lhs_adjusted = indices.first._inside ? indices.first._idx + 1 : indices.first._idx;
    size_t const between = indices.second._idx - lhs_adjusted;

    if (between != 0) {
      size_t const remaining = size() - indices.second._idx;

      std::destroy(_values + lhs_adjusted, _values + lhs_adjusted + between);

      std::move(lowers.begin() + indices.second._idx, lowers.begin() + indices.second._idx + remaining, lowers.begin() + lhs_adjusted);
      std::move(uppers.begin() + indices.second._idx, uppers.begin() + indices.second._idx + remaining, uppers.begin() + lhs_adjusted);
      std::move(_values + indices.second._idx, _values + indices.second._idx + remaining, _values + lhs_adjusted);
      _size -= between;
    }
  }
}

template <std::integral KEY_, typename VALUE_>
void IntervalMap<KEY_, VALUE_>::clear() {
  std::destroy(_values, _values + _size);
  _size = 0;
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::contains(KEY_ key_) const -> bool {
  return find(key_) != nullptr;
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::get_by_index(size_t index_) const -> Entry {
  return Entry{._interval = Interval<KEY_>(get_lowers()[index_], get_uppers()[index_]), ._value = _values[index_]};
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::find(KEY_ key_) const -> VALUE_ const* {
  IntervalIndex const index = find_interval_index<KEY_>(get_lowers(), get_uppers(), key_);
  return index._inside ? &_values[index._idx] : nullptr;
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::size() const -> size_t {
  return _size;
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::capacity() const -> size_t {
  return (_intervals == nullptr) ? 0 : AmortizedGrowth::idx_to_size(_capacity_idx);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::empty() const -> bool {
  return size() == 0;
}

template <std::integral KEY_, typename VALUE_>
void IntervalMap<KEY_, VALUE_>::reserve(size_t new_capacity_) {
  new_capacity_ = std::max(size(), new_capacity_);

  size_t const new_capacity_idx = AmortizedGrowth::size_to_idx(new_capacity_, InitialCapacityIdx);

  if (new_capacity_idx == _capacity_idx) {
    return;
  }

  size_t const old_capacity = capacity();
  new_capacity_ = AmortizedGrowth::idx_to_size(new_capacity_idx);
  _capacity_idx = new_capacity_idx;
  realloc_unique_ptr(_intervals, old_capacity * 2, new_capacity_ * 2);

  VALUE_* new_vals = static_cast<VALUE_*>(::operator new[](sizeof(VALUE_) * new_capacity_));
  for (size_t i = 0; i < _size; ++i) {
    new (&new_vals[i]) VALUE_(std::move(_values[i]));
    _values[i].~VALUE_();
  }
  ::operator delete[](_values);
  _values = new_vals;

  /* Shift up the upper bounds */
  std::move(_intervals.get() + old_capacity, _intervals.get() + old_capacity + _size, _intervals.get() + new_capacity_);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::lowest() const -> KEY_ {
  assert(!empty());
  return get_lowers()[0];
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::highest() const -> KEY_ {
  assert(!empty());
  return get_uppers()[size() - 1];
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::get_lowers() const -> IntegralSpanConst<KEY_> {
  return IntegralSpanConst<KEY_>(_intervals.get(), _size);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::get_lowers() -> IntegralSpan<KEY_> {
  return IntegralSpan<KEY_>(_intervals.get(), _size);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::get_uppers() const -> IntegralSpanConst<KEY_> {
  return IntegralSpanConst<KEY_>(_intervals.get() + capacity(), _size);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::get_uppers() -> IntegralSpan<KEY_> {
  return IntegralSpan<KEY_>(_intervals.get() + capacity(), _size);
}

template <std::integral KEY_, typename VALUE_>
auto IntervalMap<KEY_, VALUE_>::find_and_expand_interval_indices(Interval<KEY_> interval_, VALUE_ const& value_) -> IntervalIndexPair {
  IntegralSpan<KEY_> const lowers = get_lowers();
  IntegralSpan<KEY_> const uppers = get_uppers();
  IntervalIndexPair indices = find_interval_index_pair<KEY_>(lowers, uppers, interval_);

  if (!indices.first._inside && indices.first._idx != 0) {
    if (uppers[indices.first._idx - 1] == interval_.get_lower() - 1 && _values[indices.first._idx - 1] == value_) {
      --indices.first._idx;
      indices.first._inside = true;
      ++uppers[indices.first._idx];
    }
  }

  if (!indices.second._inside && indices.second._idx != size()) {
    if (lowers[indices.second._idx] == interval_.get_upper() + 1 && _values[indices.second._idx] == value_) {
      indices.second._inside = true;
      --lowers[indices.second._idx];
    }
  }

  return indices;
}

}  // namespace pmt::base