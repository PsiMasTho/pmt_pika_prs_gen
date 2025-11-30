#pragma once

#include <array>
#include <cstdint>

#include "pmt/util/sm/primitives.hpp"

namespace pmt::parser::builder {

class PackedSymbol {
 std::array<uint32_t, 2> _data;

public:
 PackedSymbol(pmt::util::sm::SymbolValueType value_);
 PackedSymbol(uint32_t lower_, uint32_t upper_);

 auto get_packed() const -> pmt::util::sm::SymbolValueType;
 auto get_unpacked_lower() const -> pmt::util::sm::SymbolValueType;
 auto get_unpacked_upper() const -> pmt::util::sm::SymbolValueType;
};

}  // namespace pmt::parser::builder