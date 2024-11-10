#include "pmt/base/dynamic_bitset_converter.hpp"

namespace pmt::base {

auto DynamicBitsetConverter::to_string(DynamicBitset const& bitset_) -> std::string {
  std::string ret;
  ret.reserve(bitset_.size());
  for (size_t i = 0; i < bitset_.size(); ++i) {
    ret.push_back(bitset_.get(i) ? '1' : '0');
  }
  return ret;
}

auto DynamicBitsetConverter::from_string(std::string const& str_) -> DynamicBitset {
  DynamicBitset ret(str_.size());
  for (size_t i = 0; i < str_.size(); ++i) {
    ret.set(i, str_[i] == '1');
  }
  return ret;
}

auto DynamicBitsetConverter::to_set(DynamicBitset const& bitset_) -> std::set<size_t> {
  std::set<size_t> ret;
  for (size_t i = 0; i < bitset_.size(); ++i) {
    if (bitset_.get(i)) {
      ret.insert(i);
    }
  }
  return ret;
}

auto DynamicBitsetConverter::from_set(std::set<size_t> const& set_, size_t size_) -> DynamicBitset {
  DynamicBitset ret(size_);
  for (size_t i : set_) {
    ret.set(i, true);
  }
  return ret;
}

auto DynamicBitsetConverter::to_unordered_set(DynamicBitset const& bitset_) -> std::unordered_set<size_t> {
  std::unordered_set<size_t> ret;
  ret.reserve(bitset_.popcnt());
  for (size_t i = 0; i < bitset_.size(); ++i) {
    if (bitset_.get(i)) {
      ret.insert(i);
    }
  }
  return ret;
}

auto DynamicBitsetConverter::from_unordered_set(std::unordered_set<size_t> const& set_, size_t size_) -> DynamicBitset {
  DynamicBitset ret(size_);
  for (size_t i : set_) {
    ret.set(i, true);
  }
  return ret;
}

}  // namespace pmt::base