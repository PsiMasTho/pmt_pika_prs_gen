#pragma once

#include <cstdint>

namespace pmt::parser {

class SourcePosition {
 public:
  // -$ Types / Constants $-
  using ColnoType = uint32_t;
  using LinenoType = uint32_t;

  // -$ Data $-
  LinenoType _lineno = 0;
  ColnoType _colno = 0;

  // -$ Functions $-
  // --$ Lifetime $--
  SourcePosition(LinenoType lineno_, ColnoType colno_);
  SourcePosition() = default;
};

}  // namespace pmt::parser