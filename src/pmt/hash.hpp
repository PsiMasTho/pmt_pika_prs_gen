#pragma once

#include <cstddef>
#include <cstdint>

namespace pmt {

class Hash {
public:
 // -$ Types / Constants $-
 enum : uint64_t {
  /* (2^N / golden ratio) for different widths, use them for hash combining */
  Phi64 = 0x9E3779B97f4A7C15U,
  Phi32 = 0x9E3779B9U,
  Phi16 = 0x9E37U,
  Phi8 = 0x3EU
 };
 // -$ Functions $-
 // --$ Other $--

 static void combine(size_t hash_, size_t& seed_);

 template <typename T_>
 static void combine(const T_& v_, size_t& seed_);
};

}  // namespace pmt

#include "pmt/hash-inl.hpp"