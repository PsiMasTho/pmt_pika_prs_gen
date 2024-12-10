// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/parsert/raw_bitset.hpp"
#endif
// clang-format on

#include <bit>
#include <numeric>

namespace pmt::util::parsert {

template <typename CHUNK_TYPE_>
auto RawBitset<CHUNK_TYPE_>::get_required_chunk_count(size_t size_in_bits_) -> size_t {
  return (size_in_bits_ + CHUNK_TYPE_SIZE - 1) / CHUNK_TYPE_SIZE;
}

template <typename CHUNK_TYPE_>
void RawBitset<CHUNK_TYPE_>::op_and(CHUNK_TYPE_ const* lhs_, CHUNK_TYPE_ const* rhs_, CHUNK_TYPE_* out_, size_t size_in_bits_) {
  for (size_t i = 0; i < get_required_chunk_count(size_in_bits_); ++i) {
    out_[i] = lhs_[i] & rhs_[i];
  }
}

template <typename CHUNK_TYPE_>
auto RawBitset<CHUNK_TYPE_>::find_first_bit(CHUNK_TYPE_ const* data_, size_t size_in_bits_) -> size_t {
  size_t total = 0;
  for (size_t i = 0; i < get_required_chunk_count(size_in_bits_); ++i) {
    size_t const incr = std::countr_zero(data_[i]);
    total += incr;
    if (incr != CHUNK_TYPE_SIZE) {
      break;
    }
  }

  return total;
}

template <typename CHUNK_TYPE_>
auto RawBitset<CHUNK_TYPE_>::popcnt(CHUNK_TYPE_ const* data_, size_t size_in_bits_) -> size_t {
  return std::accumulate(data_, data_ + get_required_chunk_count(size_in_bits_), 0, [](size_t acc_, CHUNK_TYPE_ chunk_) { return acc_ + std::popcount(chunk_); });
}

}  // namespace pmt::util::parsert