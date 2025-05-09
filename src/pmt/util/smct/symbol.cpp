#include "pmt/util/smct/symbol.hpp"

#include "pmt/base/hash.hpp"

#include <cassert>

namespace pmt::util::smct {
using namespace pmt::util::smrt;

namespace {
auto extract_kind(SymbolType combined_) -> SymbolType {
  SymbolType const ret = combined_ >> SymbolValueBitWidth;
  assert(ret <= SymbolKindMax);
  return ret;
}

auto extract_value(SymbolType combined_) -> SymbolType {
  SymbolType const ret = combined_ & ((1ull << SymbolValueBitWidth) - 1ull);
  assert(ret <= SymbolValueMax);
  return ret;
}

auto create_combined(SymbolType kind_, SymbolType value_) -> SymbolType {
  assert(kind_ <= SymbolKindMax);
  assert(value_ <= SymbolValueMax);
  return (kind_ << SymbolValueBitWidth) | value_;
}

}  // namespace

Symbol::Symbol(SymbolType combined_)
 : _kind(extract_kind(combined_))
 , _value(extract_value(combined_)) {
}

Symbol::Symbol(SymbolType kind_, SymbolType value_)
 : _kind(kind_)
 , _value(value_) {
  assert(kind_ <= SymbolKindMax);
  assert(value_ <= SymbolValueMax);
}

auto Symbol::hash() const -> size_t {
  size_t seed = pmt::base::Hash::Phi64;
  pmt::base::Hash::combine(_kind, seed);
  pmt::base::Hash::combine(_value, seed);
  return seed;
}

auto Symbol::get_kind() const -> SymbolType {
  return _kind;
}

auto Symbol::get_value() const -> SymbolType {
  return _value;
}

auto Symbol::get_combined() const -> SymbolType {
  return create_combined(_kind, _value);
}

void Symbol::set_kind(SymbolType kind_) {
  assert(kind_ <= SymbolKindMax);
  _kind = kind_;
}

void Symbol::set_value(SymbolType value_) {
  assert(value_ <= SymbolValueMax);
  _value = value_;
}

}  // namespace pmt::util::smct