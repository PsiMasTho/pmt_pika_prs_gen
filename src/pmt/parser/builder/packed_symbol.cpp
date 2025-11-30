#include "pmt/parser/builder/packed_symbol.hpp"

#include <bit>
#include <climits>

namespace pmt::parser::builder {

using namespace pmt::util::sm;

PackedSymbol::PackedSymbol(SymbolValueType value_)
 : _data(std::bit_cast<decltype(_data)>(value_)) {
}

PackedSymbol::PackedSymbol(uint32_t lower_, uint32_t upper_)
 : _data{upper_, lower_} {
}

auto PackedSymbol::get_packed() const -> SymbolValueType {
 return std::bit_cast<SymbolValueType>(_data);
}

auto PackedSymbol::get_unpacked_lower() const -> SymbolValueType {
 return _data.back();
}

auto PackedSymbol::get_unpacked_upper() const -> SymbolValueType {
 return _data.front();
}

}  // namespace pmt::parser::builder