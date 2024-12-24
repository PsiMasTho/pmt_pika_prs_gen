#pragma once

#include <climits>
#include <cstdint>

namespace pmt::util::parsert {

class GenericTablesBase {
 public:
  // - Types -
  using TableIndexType = uint64_t;

  // - Constants -
  static inline constexpr TableIndexType SYMBOL_EOI = UCHAR_MAX + 1;
};

}  // namespace pmt::util::parsert