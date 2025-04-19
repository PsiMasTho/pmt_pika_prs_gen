#pragma once

#include "pmt/base/bitset.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace pmt::base {

template <typename T_, typename U_>
  requires(std::is_integral_v<typename U_::value_type>)
void apply_permutation(T_ data_begin_, T_ data_end_, U_ indices_begin_);

template <typename T_>
  requires(std::is_integral_v<typename T_::value_type>)
void apply_permutation(Bitset& data_, T_ indices_begin_);

template <typename T_, typename U_>
  requires(std::is_integral_v<typename T_::value_type> && std::is_integral_v<typename U_::value_type>)
void inverse_permutation(T_ indices_begin_, T_ indices_end_, U_ result_begin_);

template <typename T_>
  requires(std::is_integral_v<typename T_::value_type>)
auto inverse_permutation(T_ indices_begin_, T_ indices_end_) -> std::vector<typename T_::value_type>;

template <typename T_, typename U_, typename CMP_ = std::less<>>
auto binary_find_index(T_ begin_, T_ end_, U_ const& value_, CMP_ cmp_ = CMP_{}) -> std::optional<size_t>;

template <typename T_>
void realloc_unique_ptr(std::unique_ptr<T_[]>& ptr_, size_t old_size_, size_t new_size_);

template <typename T_>
auto move_bidir(T_ data_begin_, T_ data_end_, T_ dest_);

}  // namespace pmt::base

#include "pmt/base/algo-inl.hpp"