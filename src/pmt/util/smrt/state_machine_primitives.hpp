#pragma once

#include <cstdint>
#include <limits>

namespace pmt::util::smrt {

using SymbolType = uint64_t;
using StateNrType = uint64_t;

enum : StateNrType {
  StateNrStart = 0,
  StateNrSink = std::numeric_limits<StateNrType>::max() - 25,
};

enum : SymbolType {
  SymbolEoi = std::numeric_limits<SymbolType>::max() - 25,
};

}  // namespace pmt::util::smrt