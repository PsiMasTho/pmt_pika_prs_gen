#pragma once

#include <cstddef>

namespace pmt::base {

class Hash {
 public:
  static auto fnv1a(unsigned char const* data_, size_t size_) -> size_t;

  static void combine(size_t hash_, size_t& seed_);
};

}  // namespace pmt::base