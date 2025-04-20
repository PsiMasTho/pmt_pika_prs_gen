#pragma once

#include <cstdint>
#include <limits>

namespace pmt {

using NamespaceIdType = uint64_t;

enum NamespaceId : NamespaceIdType {
  OFFSET = std::numeric_limits<NamespaceIdType>::max() / 0x100000000ULL,

  // clang-format off
  SharedNamespaceId   =  0 * OFFSET,
 
  Pmt                 =  1 * OFFSET,
  PmtBase             =  2 * OFFSET, 
  PmtBaseTest         =  6 * OFFSET,

  // clang-format on
};

}  // namespace pmt