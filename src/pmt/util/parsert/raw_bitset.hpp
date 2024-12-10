#pragma once

#include <climits>
#include <cstddef>

namespace pmt::util::parsert {

template <typename CHUNK_TYPE_>
class RawBitset {
 public:
  // - Constants -
  static size_t const CHUNK_TYPE_SIZE = sizeof(CHUNK_TYPE_) * CHAR_BIT;

  // - Static functions -
  static auto get_required_chunk_count(size_t size_in_bits_) -> size_t;

  static void op_and(CHUNK_TYPE_ const* lhs_, CHUNK_TYPE_ const* rhs_, CHUNK_TYPE_* out_, size_t size_in_bits_);
  static auto find_first_bit(CHUNK_TYPE_ const* data_, size_t size_in_bits_) -> size_t;
  static auto popcnt(CHUNK_TYPE_ const* data_, size_t size_in_bits_) -> size_t;
};

}  // namespace pmt::util::parsert

#include "pmt/util/parsert/raw_bitset-inl.hpp"