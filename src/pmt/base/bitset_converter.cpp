#include "pmt/base/bitset_converter.hpp"

namespace pmt::base {

auto BitsetConverter::to_string(Bitset const& bitset_) -> std::string {
 std::string ret;
 ret.reserve(bitset_.size());
 for (size_t i = 0; i < bitset_.size(); ++i) {
  ret.push_back(bitset_.get(i) ? '1' : '0');
 }
 return ret;
}

auto BitsetConverter::from_string(std::string const& str_) -> Bitset {
 Bitset ret(str_.size());
 for (size_t i = 0; i < str_.size(); ++i) {
  ret.set(i, str_[i] == '1');
 }
 return ret;
}

BitsetConverterError::BitsetConverterError()
 : std::logic_error("invalid dynamic bitset conversion") {
}

}  // namespace pmt::base