#pragma once

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/interval_set.hpp"

#include <concepts>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace pmt::base {

class DynamicBitsetConverter {
 public:
  // -$ Functions $-
  // --$ Other $--
  // --- String Conversion ---
  static auto to_string(DynamicBitset const& bitset_) -> std::string;
  static auto from_string(std::string const& str_) -> DynamicBitset;

  // --- Set Conversion ---
  template <std::integral T_ = size_t>
  static auto to_set(DynamicBitset const& bitset_) -> std::set<T_>;
  template <std::integral T_ = size_t>
  static auto from_set(std::set<T_> const& set_) -> DynamicBitset;

  // --- Unordered Set Conversion ---
  template <std::integral T_ = size_t>
  static auto to_unordered_set(DynamicBitset const& bitset_) -> std::unordered_set<T_>;
  template <std::integral T_ = size_t>
  static auto from_unordered_set(std::unordered_set<T_> const& set_) -> DynamicBitset;

  // --- Interval Set Conversion ---
  template <std::integral T_ = size_t>
  static auto to_interval_set(DynamicBitset const& bitset_) -> IntervalSet<T_>;
  template <std::integral T_ = size_t>
  static auto from_interval_set(IntervalSet<T_> const& interval_set_) -> DynamicBitset;
};

class DynamicBitsetConverterError : public std::logic_error {
 public:
  DynamicBitsetConverterError();
};

}  // namespace pmt::base

#include "pmt/base/dynamic_bitset_converter-inl.hpp"