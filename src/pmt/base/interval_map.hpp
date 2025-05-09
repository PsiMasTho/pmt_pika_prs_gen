#pragma once

#include "pmt/base/amortized_growth.hpp"
#include "pmt/base/interval_container_common.hpp"

#include <concepts>
#include <memory>

namespace pmt::base {

template <std::integral KEY_, typename VALUE_>
class IntervalMap {
 public:
  // -$ Types / Constants $-
  using KeyType = KEY_;
  using ValueType = VALUE_;

 private:
  enum : size_t {
    InitialCapacityIdx = 3
  };

  // -$ Data $-
  std::unique_ptr<KEY_[]> _intervals = nullptr;
  VALUE_* _values = nullptr;
  size_t _size : AmortizedGrowth::MaxCapacityBitWidth = 0;
  size_t _capacity_idx : AmortizedGrowth::MaxCapacityIdxBitWidth = 0;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  IntervalMap() = default;
  IntervalMap(IntervalMap const&);
  IntervalMap(IntervalMap&&) noexcept;
  ~IntervalMap();

  // --$ Operators $--
  auto operator=(IntervalMap const&) -> IntervalMap&;
  auto operator=(IntervalMap&&) noexcept -> IntervalMap&;
  auto operator==(IntervalMap const&) const -> bool;
  auto operator!=(IntervalMap const&) const -> bool;

  // --$ Other $--
  void insert(Interval<KEY_> interval_, VALUE_ value_);
  void erase(Interval<KEY_> interval_);
  void clear();
  auto contains(KEY_ key_) const -> bool;
  auto get_by_index(size_t index_) const -> std::pair<VALUE_ const&, Interval<KEY_>>;
  auto find(KEY_ key_) const -> VALUE_ const*;
  auto size() const -> size_t;
  auto capacity() const -> size_t;
  auto empty() const -> bool;
  void reserve(size_t new_capacity_);

  // undefined behavior if the container is empty
  auto lowest() const -> KEY_;
  auto highest() const -> KEY_;

  // iteration
  template <std::invocable<VALUE_ const&, Interval<KEY_>> F_>
  void for_each_interval(F_&& f_) const;
  
  template <std::invocable<VALUE_ const&, KEY_> F_>
  void for_each_key(F_&& f_) const;

 private:
  auto get_lowers() const -> IntegralSpanConst<KEY_>;
  auto get_lowers() -> IntegralSpan<KEY_>;
  auto get_uppers() const -> IntegralSpanConst<KEY_>;
  auto get_uppers() -> IntegralSpan<KEY_>;

  auto find_and_expand_interval_indices(Interval<KEY_> interval_, VALUE_ const& value_) -> IntervalIndexPair;
};

}  // namespace pmt::base

#include "pmt/base/interval_map-inl.hpp"