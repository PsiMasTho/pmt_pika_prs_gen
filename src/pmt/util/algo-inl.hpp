// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/algo.hpp"
#endif
// clang-format on

#include <algorithm>

namespace pmt::util {

template <typename T_, typename U_, typename CMP_>
auto binary_find_index(T_ begin_, T_ end_, U_ const& value_, CMP_ cmp_) -> size_t {
 auto const itr = std::lower_bound(begin_, end_, value_, cmp_);
 if (itr == end_ || cmp_(value_, *itr)) {
  return static_cast<size_t>(std::distance(begin_, end_));
 }
 return static_cast<size_t>(std::distance(begin_, itr));
}

template <typename T_>
void realloc_unique_ptr(std::unique_ptr<T_[]>& ptr_, size_t old_size_, size_t new_size_) {
 auto new_ptr = std::make_unique_for_overwrite<T_[]>(new_size_);
 std::copy(ptr_.get(), ptr_.get() + std::min(old_size_, new_size_), new_ptr.get());
 ptr_ = std::move(new_ptr);
}

template <typename T_>
auto move_bidir(T_ data_begin_, T_ data_end_, T_ dest_) {
 if (dest_ < data_begin_) {
  return std::move(data_begin_, data_end_, dest_);
 } else if (dest_ > data_begin_) {
  return std::move_backward(data_begin_, data_end_, dest_ + (data_end_ - data_begin_));
 } else {
  return dest_;
 }
}

}  // namespace pmt::util