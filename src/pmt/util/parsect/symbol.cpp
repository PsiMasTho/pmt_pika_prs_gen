#include "pmt/util/parsect/symbol.hpp"

#include "pmt/base/hash.hpp"

namespace pmt::util::parsect {

Symbol::Symbol(KindType kind_, ValueType value_)
 : _kind(kind_)
 , _value(value_) {
}

auto Symbol::hash() const -> size_t {
  size_t seed = 0;
  pmt::base::Hash::combine(_kind, seed);
  pmt::base::Hash::combine(_value, seed);
  return seed;
}

}  // namespace pmt::util::parsect