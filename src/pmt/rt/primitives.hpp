#pragma once

#include <cstdint>
#include <limits>

namespace pmt::rt {

using SymbolType = uint8_t;
using StateNrType = uint32_t;
using FinalIdType = uint64_t;

enum : SymbolType {
 SymbolMax = std::numeric_limits<SymbolType>::max(),
};

enum : StateNrType {
 StateNrStart = 0,
 StateNrInvalid = std::numeric_limits<StateNrType>::max(),
};

}  // namespace pmt::rt