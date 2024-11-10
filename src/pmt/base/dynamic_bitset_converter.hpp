#pragma once

#include "pmt/base/dynamic_bitset.hpp"

#include <set>
#include <string>
#include <unordered_set>

namespace pmt::base {

class DynamicBitsetConverter {
 public:
  // - String conversion -
  static auto to_string(DynamicBitset const& bitset_) -> std::string;
  static auto from_string(std::string const& str_) -> DynamicBitset;

  // - Set conversion -
  static auto to_set(DynamicBitset const& bitset_) -> std::set<size_t>;
  static auto from_set(std::set<size_t> const& set_, size_t size_) -> DynamicBitset;

  // - Unordered set conversion -
  static auto to_unordered_set(DynamicBitset const& bitset_) -> std::unordered_set<size_t>;
  static auto from_unordered_set(std::unordered_set<size_t> const& set_, size_t size_) -> DynamicBitset;
};
}  // namespace pmt::base