#include "pmt/util/uint_to_str.hpp"

#include <algorithm>
#include <cassert>

namespace pmt::util {

namespace {}

auto uint_to_string(uint64_t value_, size_t width_, std::string_view alphabet_) -> std::string {
 assert(alphabet_.size() >= min_value_base);

 std::string ret;

 if (value_ == 0) {
  ret += alphabet_.front();
 } else {
  while (value_ != 0) {
   ret.push_back(alphabet_[value_ % alphabet_.size()]);
   value_ /= alphabet_.size();
  }
 }

 ret.append((ret.size() < width_) ? width_ - ret.size() : 0, alphabet_.front());
 std::ranges::reverse(ret);
 return ret;
}

auto digits_needed(uint64_t value_, uint64_t base_) -> size_t {
 assert(base_ >= min_value_base);

 if (value_ == 0) {
  return 1;
 }

 uint64_t ret = 0;
 while (value_ > 0) {
  value_ /= base_;
  ++ret;
 }
 return ret;
}

}  // namespace pmt::util