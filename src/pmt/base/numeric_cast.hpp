#pragma once

#include <cassert>
#include <concepts>

namespace pmt::base {

class NumericCast {
public:
 /// \brief Casts a value from one integral type to another, ensuring that the
 /// value is preserved. Check is only performed in debug mode.
 template <std::integral TO_, std::integral FROM_>
 static auto cast(FROM_ from_) -> TO_ {
  TO_ result = static_cast<TO_>(from_);
  assert(static_cast<FROM_>(result) == from_ && (result >= TO_()) == (from_ >= FROM_()));
  return result;
 }
};

}  // namespace pmt::base