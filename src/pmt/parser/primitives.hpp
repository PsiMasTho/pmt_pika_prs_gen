#pragma once

#include "pmt/util/sm/primitives.hpp"

namespace pmt::parser {

enum : pmt::util::sm::SymbolKindType {
 SymbolKindCharacter = 0ull,
 SymbolKindHiddenCharacter = 6ull,
 SymbolKindTerminal = 1ull,
 SymbolKindHiddenTerminal = 2ull,
 SymbolKindOpen = 3ull,
 SymbolKindClose = 4ull,
 SymbolKindConflict = 5ull,
};

enum : pmt::util::sm::SymbolValueType {
 SymbolValueEoi = pmt::util::sm::SymbolValueMax,
 SymbolValueOpen = 0ull,
};

using SymbolType = uint64_t;

}  // namespace pmt::parser