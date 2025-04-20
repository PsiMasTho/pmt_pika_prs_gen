#pragma once

#include <cstdint>

namespace pmt::util::smrt {

class SourcePosition {
 public:
  // - Types -
  using ColnoType = uint32_t;
  using LinenoType = uint32_t;

  // - Data -
  LinenoType _lineno;
  ColnoType _colno;

  // - Member functions -
  SourcePosition(LinenoType lineno_, ColnoType colno_);
  SourcePosition() = default;
};

}  // namespace pmt::util::smrt