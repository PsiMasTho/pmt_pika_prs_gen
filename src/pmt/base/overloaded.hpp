#pragma once

namespace pmt::base {

template <typename... TS_>
class overloaded : TS_... {
 public:
  using TS_::operator()...;
};

template <typename... TS_>
overloaded(TS_...) -> overloaded<TS_...>;

}  // namespace pmt::base