#pragma once

#include <bit>
#include <cstdint>

namespace pmt::util {

// Actually force the values to be 0 or 1, because the standard values could be anything
enum endianness : std::uint8_t {
  LITTLE = 0,
  BIG = 1,
  NATIVE = std::endian::native == std::endian::little ? LITTLE : BIG
};

}  // namespace pmt::util