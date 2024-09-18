#pragma once

#include <chrono>
#include <optional>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace pmt::base {

class std_type_traits {
  template <typename... T_>
  struct is_vector : std::false_type {};

  template <typename... T_>
  struct is_vector<std::vector<T_...>> : std::true_type {};

  template <typename... T_>
  struct is_tuple : std::false_type {};

  template <typename... T_>
  struct is_tuple<std::tuple<T_...>> : std::true_type {};

  template <typename T_>
  struct is_optional : std::false_type {};

  template <typename T_>
  struct is_optional<std::optional<T_>> : std::true_type {};

  template <typename T_>
  struct is_variant : std::false_type {};

  template <typename... TS_>
  struct is_variant<std::variant<TS_...>> : std::true_type {};

  template <typename T_>
  struct is_duration : std::false_type {};

  template <typename REP_, typename PERIOD_>
  struct is_duration<std::chrono::duration<REP_, PERIOD_>> : std::true_type {};
};

}  // namespace pmt::base