#pragma once

#include <cstdint>
#include <limits>
#include <climits>

namespace pmt::util::smrt {

using SymbolType = uint64_t;
using StateNrType = uint64_t;

enum : StateNrType {
  StateNrStart = 0,
  StateNrSink = std::numeric_limits<StateNrType>::max(),
};

enum : SymbolType {
 SymbolKindBitWidth = 8ull,
 SymbolValueBitWidth = sizeof(pmt::util::smrt::SymbolType) * CHAR_BIT - SymbolKindBitWidth,
 SymbolKindMax = (1ull << SymbolKindBitWidth) - 1ull,
 SymbolValueMax = (1ull << SymbolValueBitWidth) - 1ull,
 
 SymbolValueEoi = SymbolValueMax,
 SymbolValueOpen = 0ull,

 SymbolKindCharacter = 0ull,
 SymbolKindTerminal = 1ull,
 SymbolKindOpen = 2ull,
 SymbolKindClose = 3ull,
};

}  // namespace pmt::util::smrt