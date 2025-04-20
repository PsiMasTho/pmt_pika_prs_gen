#pragma once

#include "pmt/base/amortized_growth.hpp"
#include "pmt/base/hashable.hpp"
#include "pmt/base/interval_container_common.hpp"

#include <concepts>
#include <memory>

namespace pmt::base {

template <std::integral KEY_>
class IntervalSet : public Hashable<IntervalSet<KEY_>> {
 public:
  // -$ Types / Constants $-
  using KeyType = KEY_;

 private:
  enum : size_t {
    InitialCapacityIdx = 3
  };

  // -$ Data $-
  std::unique_ptr<KEY_[]> _intervals = nullptr;
  size_t _size : AmortizedGrowth::MaxCapacityBitWidth = 0;
  size_t _capacity_idx : AmortizedGrowth::MaxCapacityIdxBitWidth = 0;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  IntervalSet() = default;
  IntervalSet(IntervalSet const&);
  IntervalSet(IntervalSet&&) noexcept = default;
  ~IntervalSet() = default;

  // --$ Operators $--
  auto operator=(IntervalSet const&) -> IntervalSet&;
  auto operator=(IntervalSet&&) noexcept -> IntervalSet& = default;
  auto operator==(IntervalSet const&) const -> bool;
  auto operator!=(IntervalSet const&) const -> bool;

  // --$ Inherited: pmt::base::Hashable $--
  auto hash() const -> size_t;

  // --$ Other $--
  void insert(Interval<KEY_> interval_);
  void erase(Interval<KEY_> interval_);
  void clear();
  auto contains(KEY_ key_) const -> bool;
  auto get_by_index(size_t index_) const -> Interval<KEY_>;
  auto size() const -> size_t;
  auto capacity() const -> size_t;
  auto empty() const -> bool;
  void reserve(size_t new_capacity_);

  // bitwise operations
  auto popcnt() const -> size_t;
  void inplace_or(IntervalSet const& other_);


  auto clone_and(IntervalSet const& other_) const -> IntervalSet;
  auto clone_asymmetric_difference(IntervalSet const& other_) const -> IntervalSet;

  // undefined behavior if the container is empty
  auto lowest() const -> KEY_;
  auto highest() const -> KEY_;

 private:
  auto get_lowers() const -> IntegralSpanConst<KEY_>;
  auto get_lowers() -> IntegralSpan<KEY_>;
  auto get_uppers() const -> IntegralSpanConst<KEY_>;
  auto get_uppers() -> IntegralSpan<KEY_>;

  auto find_and_expand_interval_indices(Interval<KEY_> interval_) -> IntervalIndexPair;
};

}  // namespace pmt::base

#include "pmt/base/interval_set-inl.hpp"