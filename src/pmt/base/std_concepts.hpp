#pragma once

#include "pmt/base/std_type_traits.hpp"

namespace pmt::base {

template <typename T_>
concept is_std_vector = std_type_traits::is_vector<T_>::value;

template <typename T_>
concept is_std_tuple = std_type_traits::is_tuple<T_>::value;

template <typename T_>
concept is_std_optional = std_type_traits::is_optional<T_>::value;

template <typename T_>
concept is_std_variant = std_type_traits::is_variant<T_>::value;

template <typename T_>
concept is_std_duration = std_type_traits::is_duration<T_>::value;

}  // namespace pmt::base