#pragma once

#include <functional>
#include <tuple>
#include <vector>

namespace pmt::base {

template <typename KEY_, typename VALUE_, typename KEY_CMP_ = std::less<KEY_>, typename VALUE_EQ_ = std::equal_to<VALUE_>>
class RangeMap {
 public:
  // -$ Types / Constants $-
  using ValueType = VALUE_;
  using KeyType = KEY_;
  using KeyCompareType = KEY_CMP_;
  using ValueEqualType = VALUE_EQ_;

 private:
  // -$ Data $-
  std::vector<KEY_> _lowers;
  std::vector<KEY_> _uppers;
  std::vector<VALUE_> _values;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  RangeMap() = default;

  // --$ Operators $--
  auto operator==(const RangeMap&) const -> bool = default;

  // --$ Other $--
  template <typename KEY_T_1_, typename KEY_T_2_, typename VALUE_T_>
  void insert(KEY_T_1_ lower_, KEY_T_2_ upper_, VALUE_T_ value_);

  template <typename KEY_T_, typename VALUE_T_>
  void insert(KEY_T_ key_, VALUE_T_ value_);

  template <typename KEY_T_1_, typename KEY_T_2_, typename... ARGS_>
  void emplace(KEY_T_1_ lower_, KEY_T_2_ upper_, ARGS_&&... args_);

  template <typename KEY_T_, typename... ARGS_>
  void emplace(KEY_T_ key_, ARGS_&&... args_);

  void erase(KEY_ const& lower_, KEY_ const& upper_);
  void erase(KEY_ const& key_);

  void clear();

  auto find(KEY_ const& key_) const -> VALUE_ const*;

  auto get(size_t index_) -> std::tuple<KEY_ const&, KEY_ const&, VALUE_ const&>;

  auto size() const -> size_t;
  auto empty() const -> bool;
  void reserve(size_t size_);
};

}  // namespace pmt::base

#include "range_map-inl.hpp"