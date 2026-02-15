#pragma once

#include <cstdint>
#include <limits>

namespace pmt::pika::rt {

using SymbolType = uint8_t;
using StateNrType = uint32_t;
using IdType = uint64_t;

enum : StateNrType {
 StateNrStart = 0,
 StateNrInvalid = std::numeric_limits<StateNrType>::max(),
};

}  // namespace pmt::pika::rt