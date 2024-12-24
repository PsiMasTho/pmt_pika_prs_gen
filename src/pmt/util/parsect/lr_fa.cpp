#include "pmt/util/parsect/lr_fa.hpp"

#include "pmt/base/hash.hpp"

namespace pmt::util::parsect {

auto LrFa::Symbol::hash() const -> size_t {
  size_t seed = 0;
  pmt::base::Hash::combine(_index, seed);
  pmt::base::Hash::combine(_type, seed);
  return seed;
}

auto LrFa::Symbol::operator<(Symbol const& other_) const -> bool {
  if (_type < other_._type) {
    return true;
  }
  if (_type > other_._type) {
    return false;
  }

  return _index < other_._index;
}

}  // namespace pmt::util::parsect