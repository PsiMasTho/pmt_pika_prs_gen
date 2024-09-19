#pragma once

#include <cstdint>
#include <limits>

namespace pmt {

using namespace_id_type = uint64_t;

enum namespace_id : namespace_id_type {
  OFFSET = std::numeric_limits<namespace_id_type>::max() / 0x100000000ULL,

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