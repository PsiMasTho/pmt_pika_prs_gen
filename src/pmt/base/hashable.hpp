#pragma once

#include <cstddef>
#include <functional>

namespace pmt::base {

template <typename CRTP_>
class Hashable {
  Hashable() = default;
  ~Hashable() = default;
  friend CRTP_;

 public:
  auto operator<=>(const Hashable& other_) const -> bool = default;

  auto hash() const -> size_t;
};

}  // namespace pmt::base

namespace std {
template <typename CRTP_>
  requires std::is_base_of_v<pmt::base::Hashable<CRTP_>, CRTP_>
struct hash<CRTP_> {
  auto operator()(CRTP_ const& hashable_) const -> size_t {
    return hashable_.hash();
  }
};
}  // namespace std

#include "pmt/base/hashable-inl.hpp"