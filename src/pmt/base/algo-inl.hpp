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
  DynamicBitset visited(size, false);
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
void apply_permutation(DynamicBitset& data_, T_ indices_begin_) {
  DynamicBitset visited(data_.size(), false);
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

template <typename T_, typename CMP_>
auto binary_find_index(T_ begin_, T_ end_, typename T_::value_type const& value_, CMP_ cmp_) -> std::optional<size_t> {
  auto const itr = std::lower_bound(begin_, end_, value_, cmp_);
  if (itr == end_ || *itr != value_) {
    return std::nullopt;
  }
  return NumericCast::cast<size_t>(std::distance(begin_, itr));
}

}  // namespace pmt::base