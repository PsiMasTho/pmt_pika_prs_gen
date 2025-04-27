// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/algo.hpp"
#endif
// clang-format on

#include "pmt/base/numeric_cast.hpp"

#include <algorithm>

namespace pmt::base {

template <typename T_, typename U_>
  requires(std::is_integral_v<typename U_::value_type>)
void apply_permutation(T_ data_begin_, T_ data_end_, U_ indices_begin_) {
  size_t const size = NumericCast::cast<size_t>(std::distance(data_begin_, data_end_));
  Bitset visited(size, false);
  for (size_t i = 0; i < size; ++i) {
    size_t current = i;
    while (!visited.get(current) && !visited.get(indices_begin_[current])) {
      size_t const next = NumericCast::cast<size_t>(indices_begin_[current]);
      std::swap(data_begin_[current], data_begin_[next]);
      visited.set(current, true);
      current = next;
    }
    visited.set(current, true);
  }
}

template <typename T_>
  requires(std::is_integral_v<typename T_::value_type>)
void apply_permutation(Bitset& data_, T_ indices_begin_) {
  Bitset visited(data_.size(), false);
  for (size_t i = 0; i < data_.size(); ++i) {
    size_t current = i;
    while (!visited.get(current) && !visited.get(indices_begin_[current])) {
      size_t const next = NumericCast::cast<size_t>(indices_begin_[current]);
      data_.swap(current, next);
      visited.set(current, true);
      current = next;
    }
    visited.set(current, true);
  }
}

template <typename T_, typename U_>
  requires(std::is_integral_v<typename T_::value_type> && std::is_integral_v<typename U_::value_type>)
void inverse_permutation(T_ indices_begin_, T_ indices_end_, U_ result_begin_) {
  size_t const size = NumericCast::cast<size_t>(std::distance(indices_begin_, indices_end_));
  for (size_t i = 0; i < size; ++i) {
    result_begin_[indices_begin_[i]] = i;
  }
}

template <typename T_>
  requires(std::is_integral_v<typename T_::value_type>)
auto inverse_permutation(T_ indices_begin_, T_ indices_end_) -> std::vector<typename T_::value_type> {
  std::vector<typename T_::value_type> ret(NumericCast::cast<size_t>(std::distance(indices_begin_, indices_end_)));
  inverse_permutation(indices_begin_, indices_end_, ret.begin());
  return ret;
}

template <typename T_, typename U_, typename CMP_>
auto binary_find_index(T_ begin_, T_ end_, U_ const& value_, CMP_ cmp_) -> size_t {
  auto const itr = std::lower_bound(begin_, end_, value_, cmp_);
  if (itr == end_ || cmp_(value_, *itr)) {
    return NumericCast::cast<size_t>(std::distance(begin_, end_));
  }
  return NumericCast::cast<size_t>(std::distance(begin_, itr));
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

}  // namespace pmt::base