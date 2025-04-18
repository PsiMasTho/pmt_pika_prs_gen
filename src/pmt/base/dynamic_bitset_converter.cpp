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

DynamicBitsetConverterError::DynamicBitsetConverterError()
 : std::logic_error("invalid dynamic bitset conversion") {
}

}  // namespace pmt::base