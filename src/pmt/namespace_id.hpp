#pragma once

#include <cstdint>
#include <limits>

namespace pmt {

enum namespace_id : uint64_t {
  OFFSET = std::numeric_limits<uint64_t>::max() / 0x100000000ULL,

  // clang-format off
 SHARED_NAMESPACE_ID =  0 * OFFSET,
 
 PMT                 =  1 * OFFSET,
 PMT_BASE            =  2 * OFFSET, 
 PMT_GENERATOR       =  3 * OFFSET,
 PMT_UTIL            =  4 * OFFSET,
 PMT_UTIL_PARSE      =  5 * OFFSET,

  // clang-format on
};

}  // namespace pmt