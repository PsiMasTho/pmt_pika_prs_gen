#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace pmt::util::sm {

using SymbolValueType = uint64_t;
using SymbolKindType = uint8_t;
using StateNrType = uint64_t;
using AcceptsIndexType = size_t;

enum : StateNrType {
 StateNrStart = 0,
 StateNrSink = std::numeric_limits<StateNrType>::max(),
};

enum : SymbolKindType {
 SymbolKindMax = std::numeric_limits<SymbolKindType>::max(),
 SymbolKindInvalid = SymbolKindMax,
};

enum : SymbolValueType {
 SymbolValueMax = std::numeric_limits<SymbolValueType>::max(),
 SymbolValueInvalid = SymbolValueMax,
};

enum : size_t {
 AcceptIndexMax = std::numeric_limits<AcceptsIndexType>::max(),
 AcceptIndexInvalid = AcceptIndexMax,
};

}  // namespace pmt::util::sm