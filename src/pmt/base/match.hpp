#pragma once

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
};

}