#pragma once

#include <chrono>
#include <optional>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace pmt::base {

class StdTypeTraits {
 template <typename... T_>
 struct IsVector : std::false_type {};

 template <typename... T_>
 struct IsVector<std::vector<T_...>> : std::true_type {};

 template <typename... T_>
 struct IsTuple : std::false_type {};

 template <typename... T_>
 struct IsTuple<std::tuple<T_...>> : std::true_type {};

 template <typename T_>
 struct IsOptional : std::false_type {};

 template <typename T_>
 struct IsOptional<std::optional<T_>> : std::true_type {};

 template <typename T_>
 struct IsVariant : std::false_type {};

 template <typename... TS_>
 struct IsVariant<std::variant<TS_...>> : std::true_type {};

 template <typename T_>
 struct IsDuration : std::false_type {};

 template <typename REP_, typename PERIOD_>
 struct IsDuration<std::chrono::duration<REP_, PERIOD_>> : std::true_type {};

 template <typename T_>
 struct IsRatio : std::false_type {};

 template <std::intmax_t NUM_, std::intmax_t DEN_>
 struct IsRatio<std::ratio<NUM_, DEN_>> : std::true_type {};
};

}  // namespace pmt::base