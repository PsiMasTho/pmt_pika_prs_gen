#include "pmt/util/parsect/lr_item.hpp"

#include "pmt/base/hash.hpp"

namespace pmt::util::parsect {}  // namespace pmt::util::parsect

namespace std {
auto hash<pmt::util::parsect::LrItem>::operator()(pmt::util::parsect::LrItem const& item_) const -> size_t {
  size_t seed = 0;
  pmt::base::Hash::combine(item_._production_index, seed);
  pmt::base::Hash::combine(item_._dot_position, seed);
  return seed;
}
}  // namespace std