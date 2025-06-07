#pragma once

#include <cstdint>
#include <limits>
#include <cstddef>

namespace pmt::util::smrt {

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

 SymbolKindCharacter = 0ull,
 SymbolKindTerminal = 1ull,
 SymbolKindHiddenTerminal = 2ull,
 SymbolKindOpen = 3ull,
 SymbolKindClose = 4ull,
 SymbolKindConflict = 5ull,
};

enum : SymbolValueType {
 SymbolValueMax = std::numeric_limits<SymbolValueType>::max(),
 
 SymbolValueEoi = SymbolValueMax,
 SymbolValueOpen = 0ull,
};

enum : size_t {
 AcceptIndexInvalid = std::numeric_limits<size_t>::max(),
};

}  // namespace pmt::util::smrt