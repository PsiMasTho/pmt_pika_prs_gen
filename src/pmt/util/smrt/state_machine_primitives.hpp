#pragma once

#include <cstdint>
#include <limits>

namespace pmt::util::smrt {

using SymbolType = uint64_t;
using StateNrType = uint64_t;

enum : StateNrType {
  StateNrStart = 0,
  StateNrSink = std::numeric_limits<StateNrType>::max(),
};

enum : SymbolType {
  SymbolEoi = std::numeric_limits<SymbolType>::max(),
};

}  // namespace pmt::util::smrt