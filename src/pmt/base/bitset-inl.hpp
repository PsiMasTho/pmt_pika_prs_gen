// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/base/bitset.hpp"
#endif
// clang-format on

namespace pmt::base {

template <std::invocable<size_t> F_>
void Bitset::for_each_bit(F_&& f_) const {
 size_t const chunk_count = get_required_chunk_count(_size);
 for (size_t i = 0; i < chunk_count; ++i) {
  while (i < chunk_count && _data[i] == ALL_SET_MASKS[false]) {
   ++i;
  }

  if (i >= chunk_count) {
   break;
  }

  ChunkType chunk = _data[i];
  size_t cur = std::countr_zero(chunk);
  while (chunk != ALL_SET_MASKS[false]) {
   f_(i * ChunkBit + cur);
   chunk &= ~(ChunkType(1) << cur);
   cur = std::countr_zero(chunk);
  }
 }
}

}  // namespace pmt::base