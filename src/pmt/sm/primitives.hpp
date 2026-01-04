#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace pmt::sm {

using SymbolType = uint64_t;
using StateNrType = uint64_t;
using AcceptsIndexType = uint64_t;

enum : StateNrType {
 StateNrStart = 0,
 StateNrInvalid = std::numeric_limits<StateNrType>::max(),
 StateNrMaxValid = StateNrInvalid - 1,
};

enum : SymbolType {
 SymbolMaxValid = std::numeric_limits<SymbolType>::max(),
};

enum : AcceptsIndexType {
 AcceptsIndexInvalid = std::numeric_limits<AcceptsIndexType>::max(),
 AcceptsIndexMaxValid = AcceptsIndexInvalid - 1,
};

enum : size_t {
 StateNrBitCount = sizeof(StateNrType) * CHAR_BIT,
 SymbolBitCount = sizeof(SymbolType) * CHAR_BIT,
 AcceptsIndexBitCount = sizeof(AcceptsIndexType) * CHAR_BIT,
};

}  // namespace pmt::sm