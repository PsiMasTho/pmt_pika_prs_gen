#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>

namespace pmt::base {

template <typename CRTP_>
class Hashable {
 public:
  auto operator<=>(Hashable const&) const = default;

  auto hash() const -> size_t;

 protected:
  Hashable() = default;
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