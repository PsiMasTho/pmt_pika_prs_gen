#pragma once

#include <memory>

namespace pmt::util {

template <typename T_, typename U_, typename CMP_ = std::less<>>
auto binary_find_index(T_ begin_, T_ end_, U_ const& value_, CMP_ cmp_ = CMP_{}) -> size_t;

template <typename T_>
void realloc_unique_ptr(std::unique_ptr<T_[]>& ptr_, size_t old_size_, size_t new_size_);

template <typename T_>
auto move_bidir(T_ data_begin_, T_ data_end_, T_ dest_);

}  // namespace pmt::util

#include "pmt/util/algo-inl.hpp"