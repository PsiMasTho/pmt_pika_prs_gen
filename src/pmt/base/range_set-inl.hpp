// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/range_set.hpp"
#endif
// clang-format on

#include <algorithm>

namespace pmt::base {

template <std::integral KEY_>
void RangeSet<KEY_>::insert(KEY_ lower_, KEY_ upper_) {
 auto const itr1 = std::upper_bound(_lowers.begin(), _lowers.end(), lower_);

 // *itr1 != end, then:
 // *itr1 is bigger than lower_
 // else
 // *itr1 is smaller than lower_

 bool inside_1 = false;
 if (itr1 != _lowers.end()) {
  if (itr1 != _lowers.begin() && *(itr1 - 1) + 1 >= lower_) {
   --itr1;
   lower_ = *itr1;
   inside_1 = true;
  } else {
   if (lower_ + 1 == *itr1) {
    *itr1 = lower_; 
    inside_1 = true;
   }
  }
 } else {
  KEY_ tmp = _uppers.back();
  tmp += (tmp == std::numeric_limits<KEY_>::max()) ? 0 : 1;
  if (tmp >= lower_) {
   
  }
 }

 size_t index_1 = std::distance(itr1, _lowers.begin());

}

template <std::integral KEY_>
void RangeSet<KEY_>::insert(KEY_ key_) {
  insert(key_, key_);
}

template <std::integral KEY_>
void RangeSet<KEY_>::erase(KEY_ lower_, KEY_ upper_) {
}

template <std::integral KEY_>
void RangeSet<KEY_>::erase(KEY_ key_) {
}

template <std::integral KEY_>
void RangeSet<KEY_>::clear() {
  _lowers.clear();
  _uppers.clear();
}

template <std::integral KEY_>
auto RangeSet<KEY_>::overlap(KEY_ lower_, KEY_ upper_) const -> RangeSet {
}

template <std::integral KEY_>
auto RangeSet<KEY_>::get(size_t index_) const -> std::tuple<KEY_, KEY_> {
  return {_lowers[index_], _uppers[index_]};
}

template <std::integral KEY_>
auto RangeSet<KEY_>::size() const -> size_t {
  return _lowers.size();
}

template <std::integral KEY_>
auto RangeSet<KEY_>::empty() const -> bool {
  return _lowers.empty();
}

template <std::integral KEY_>
void RangeSet<KEY_>::reserve(size_t size_) {
  _lowers.reserve(size_);
  _uppers.reserve(size_);
}


}  // namespace pmt::base