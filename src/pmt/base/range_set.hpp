#pragma once

#include <concepts>
#include <tuple>
#include <vector>

namespace pmt::base {

template <std::integral KEY_>
class RangeSet {
 public:
  // -$ Types / Constants $-
  using KeyType = KEY_;

 private:
  // -$ Data $-
  std::vector<KEY_> _lowers;
  std::vector<KEY_> _uppers;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  RangeSet() = default;

  // --$ Operators $--
  auto operator==(const RangeSet&) const -> bool = default;

  // --$ Other $--
  void insert(KEY_ lower_, KEY_ upper_);
  void insert(KEY_ key_);

  void erase(KEY_ lower_, KEY_ upper_);
  void erase(KEY_ key_);

  void clear();

  auto contains(KEY_ key_) const -> bool;

  auto overlap(KEY_ lower_, KEY_ upper_) const -> RangeSet;

  auto get(size_t index_) const -> std::tuple<KEY_, KEY_>;

  auto size() const -> size_t;
  auto empty() const -> bool;
  void reserve(size_t size_);

 private:
  template <typename ITR_>
  static auto find_insertion(ITR_ first_lowers_, ITR_ first_uppers_, size_t size_, KEY_ key_, auto const& key_cmp_, auto const& key_incr_) -> std::pair<ITR_, bool>;

  static auto key_incr(KEY_ key_) -> KEY_;
  static auto key_decr(KEY_ key_) -> KEY_;
  static auto key_noop(KEY_ key_) -> KEY_;
};

}  // namespace pmt::base

#include "pmt/base/range_set-inl.hpp"