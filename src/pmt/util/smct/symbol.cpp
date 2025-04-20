#include "pmt/util/smct/symbol.hpp"

#include "pmt/base/hash.hpp"

#include <cassert>

namespace pmt::util::smct {

namespace {
auto extract_kind(Symbol::UnderlyingType combined_) -> Symbol::UnderlyingType {
  Symbol::UnderlyingType const ret = combined_ >> Symbol::ValueBitWidth;
  assert(ret <= Symbol::KindMax);
  return ret;
}

auto extract_value(Symbol::UnderlyingType combined_) -> Symbol::UnderlyingType {
  Symbol::UnderlyingType const ret = combined_ & ((1ull << Symbol::ValueBitWidth) - 1ull);
  assert(ret <= Symbol::ValueMax);
  return ret;
}

auto create_combined(Symbol::UnderlyingType kind_, Symbol::UnderlyingType value_) -> Symbol::UnderlyingType {
  assert(kind_ <= Symbol::KindMax);
  assert(value_ <= Symbol::ValueMax);
  return (kind_ << Symbol::ValueBitWidth) | value_;
}

}  // namespace

Symbol::Symbol(UnderlyingType combined_)
 : _kind(extract_kind(combined_))
 , _value(extract_value(combined_)) {
}

Symbol::Symbol(UnderlyingType kind_, UnderlyingType value_)
 : _kind(kind_)
 , _value(value_) {
  assert(kind_ <= KindMax);
  assert(value_ <= ValueMax);
}

auto Symbol::hash() const -> size_t {
  size_t seed = pmt::base::Hash::Phi64;
  pmt::base::Hash::combine(_kind, seed);
  pmt::base::Hash::combine(_value, seed);
  return seed;
}

auto Symbol::get_kind() const -> UnderlyingType {
  return _kind;
}

auto Symbol::get_value() const -> UnderlyingType {
  return _value;
}

auto Symbol::get_combined() const -> UnderlyingType {
  return create_combined(_kind, _value);
}

void Symbol::set_kind(UnderlyingType kind_) {
  assert(kind_ <= KindMax);
  _kind = kind_;
}

void Symbol::set_value(UnderlyingType value_) {
  assert(value_ <= ValueMax);
  _value = value_;
}

}  // namespace pmt::util::smct