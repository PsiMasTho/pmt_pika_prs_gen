#pragma once

#include "pmt/base/interval_container_common.hpp"

namespace pmt::base {

class Match {
public:
 template <typename T_, typename... TS_>
 static constexpr auto is_any_of(T_ const& value_, TS_ const&... candidates_) -> bool {
  return ((value_ == candidates_) || ...);
 }

 template <typename T_, typename... TS_>
 static constexpr auto is_none_of(T_ const& value_, TS_ const&... candidates_) -> bool {
  return !is_any_of(value_, candidates_...);
 }

 template <std::integral T_, std::integral... TS_>
 static constexpr auto is_in_any_of(T_ value_, Interval<TS_> const&... intervals_) -> bool {
  return (is_inside_interval(value_, intervals_) || ...);
 }

 template <std::integral T_, std::integral... TS_>
 static constexpr auto is_in_none_of(T_ value_, Interval<TS_> const&... intervals_) -> bool {
  return (!is_inside_interval(value_, intervals_) && ...);
 }

 template <std::integral T_, std::integral U_>
 static constexpr auto is_inside_interval(T_ value_, Interval<U_> interval_) {
  return (value_ >= interval_.get_lower() && value_ <= interval_.get_upper());
 }
};

}  // namespace pmt::base