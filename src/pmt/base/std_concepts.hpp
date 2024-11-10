#pragma once

#include "pmt/base/std_type_traits.hpp"

namespace pmt::base {

template <typename T_>
concept IsStdVector = StdTypeTraits::IsVector<T_>::value;

template <typename T_>
concept IsStdTuple = StdTypeTraits::IsTuple<T_>::value;

template <typename T_>
concept IsStdOptional = StdTypeTraits::IsOptional<T_>::value;

template <typename T_>
concept IsStdVariant = StdTypeTraits::IsVariant<T_>::value;

template <typename T_>
concept IsStdDuration = StdTypeTraits::IsDuration<T_>::value;

template <typename T_>
concept IsStdRatio = StdTypeTraits::IsRatio<T_>::value;

}  // namespace pmt::base