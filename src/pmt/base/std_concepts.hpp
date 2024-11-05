#pragma once

#include "pmt/base/std_type_traits.hpp"

namespace pmt::base {

template <typename T_>
concept IsStdVector = std_type_traits::is_vector<T_>::value;

template <typename T_>
concept IsStdTuple = std_type_traits::is_tuple<T_>::value;

template <typename T_>
concept IsStdOptional = std_type_traits::is_optional<T_>::value;

template <typename T_>
concept IsStdVariant = std_type_traits::is_variant<T_>::value;

template <typename T_>
concept IsStdDuration = std_type_traits::is_duration<T_>::value;

}  // namespace pmt::base