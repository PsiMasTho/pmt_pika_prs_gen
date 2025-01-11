#pragma once

#include <cstddef>

namespace pmt::base {

class Hash {
 public:
  // -$ Functions $-
  // --$ Other $--
  static auto fnv1a(unsigned char const* data_, size_t size_) -> size_t;

  static void combine(size_t hash_, size_t& seed_);

  template <typename T_>
  static void combine(const T_& v_, size_t& seed_);
};

}  // namespace pmt::base

#include "pmt/base/hash-inl.hpp"