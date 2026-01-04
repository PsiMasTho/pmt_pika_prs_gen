#pragma once

#include "pmt/base/algo.hpp"
#include "pmt/sm/primitives.hpp"

#include <limits>

namespace pmt::parser {

using SymbolKindType = uint8_t;

enum : SymbolKindType {
 SymbolKindCharacter = 0ull,
 SymbolKindHiddenCharacter = 6ull,
 SymbolKindTerminal = 1ull,
 SymbolKindHiddenTerminal = 2ull,
 SymbolKindOpen = 3ull,
 SymbolKindClose = 4ull,
 SymbolKindConflict = 5ull,
 SymbolKindOrderedEpsilonOpen = 7ull,
 SymbolKindOrderedEpsilonClose = 8ull,

 SymbolKindInvalid = std::numeric_limits<SymbolKindType>::max(),
 SymbolKindMaxValid = SymbolKindInvalid - 1,
};

enum : size_t {
 SymbolKindBitCount = sizeof(SymbolKindType) * CHAR_BIT,
 SymbolValueBitCount = pmt::sm::SymbolBitCount - SymbolKindBitCount,
};

enum : pmt::sm::SymbolType {
 SymbolValueInvalid = pmt::base::get_max_unsigned_value_for_bit_count(SymbolValueBitCount),
 SymbolValueMaxValid = SymbolValueInvalid - 1,
 SymbolValueEoi = SymbolValueMaxValid,
 SymbolValueOpen = 0ull,

 SymbolKindBitMask = ~0ULL << SymbolValueBitCount,
 SymbolValueBitMask = ~SymbolKindBitMask,
};

}  // namespace pmt::parser