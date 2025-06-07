#include "pmt/util/smct/symbol.hpp"

#include "pmt/base/hash.hpp"

namespace pmt::util::smct {
using namespace pmt::util::smrt;

namespace {

}  // namespace

Symbol::Symbol(SymbolKindType kind_, SymbolValueType value_)
 : _kind(kind_)
 , _value(value_) {
}

auto Symbol::hash() const -> size_t {
  size_t seed = pmt::base::Hash::Phi64;
  pmt::base::Hash::combine(_kind, seed);
  pmt::base::Hash::combine(_value, seed);
  return seed;
}

auto Symbol::get_kind() const -> SymbolKindType {
  return _kind;
}

auto Symbol::get_value() const -> SymbolValueType {
  return _value;
}

void Symbol::set_kind(SymbolKindType kind_) {
  _kind = kind_;
}

void Symbol::set_value(SymbolValueType value_) {
  _value = value_;
}

}  // namespace pmt::util::smct