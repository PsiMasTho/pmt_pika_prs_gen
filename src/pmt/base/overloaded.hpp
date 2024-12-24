#pragma once

namespace pmt::base {

template <typename... TS_>
class Overloaded : public TS_... {
 public:
  using TS_::operator()...;
};

template <typename... TS_>
Overloaded(TS_...) -> Overloaded<TS_...>;

}  // namespace pmt::base