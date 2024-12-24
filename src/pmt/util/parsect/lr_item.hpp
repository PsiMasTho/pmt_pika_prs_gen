#pragma once

#include <cstddef>
#include <functional>

namespace pmt::util::parsect {

class LrItem {
 public:
  auto operator==(LrItem const& other_) const -> bool = default;

  size_t _production_index;
  size_t _dot_position;
};

}  // namespace pmt::util::parsect

namespace std {
template <>
struct hash<pmt::util::parsect::LrItem> {
  auto operator()(pmt::util::parsect::LrItem const& item_) const -> size_t;
};

}  // namespace std