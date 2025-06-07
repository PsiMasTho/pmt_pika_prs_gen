// clang-format off
#ifdef __INTELLISENSE__
    #include "pmt/util/smrt/util.hpp"
#endif
// clang-format on

#include <algorithm>
#include <cassert>
#include <span>

namespace pmt::util::smrt {

 auto find_first_set_bit(pmt::base::Bitset::ChunkSpanConst const& bitset_) -> size_t {
  size_t total = 0;
  for (pmt::base::Bitset::ChunkType chunk : bitset_) {
    size_t const incr = std::countr_zero(chunk);
    total += incr;
    if (incr != pmt::base::Bitset::ChunkBit) {
      break;
    }
  }

  return total;
 }

 void set_bit(pmt::base::Bitset::ChunkSpan bitset_, size_t index_) {
  size_t const chunk_index = index_ / pmt::base::Bitset::ChunkBit;
  size_t const bit_index = index_ % pmt::base::Bitset::ChunkBit;

  assert(chunk_index < bitset_.size());
  bitset_[chunk_index] |= pmt::base::Bitset::ChunkType(1) << bit_index;
 }

 auto get_bit(pmt::base::Bitset::ChunkSpanConst bitset_, size_t index_) -> bool {
  size_t const chunk_index = index_ / pmt::base::Bitset::ChunkBit;
  size_t const bit_index = index_ % pmt::base::Bitset::ChunkBit;

  if (chunk_index >= bitset_.size()) {
    return false;
  }

  return (bitset_[chunk_index] & (pmt::base::Bitset::ChunkType(1) << bit_index)) != 0;
 }

 void bitwise_and(pmt::base::Bitset::ChunkSpan bitset_, pmt::base::Bitset::ChunkSpanConst lhs_ , pmt::base::Bitset::ChunkSpanConst rhs_) {
  std::fill(bitset_.begin(), bitset_.end(), pmt::base::Bitset::ChunkType(0));

  pmt::base::Bitset::ChunkSpanConst const* smaller = lhs_.size() < rhs_.size() ? &lhs_ : &rhs_;
  pmt::base::Bitset::ChunkSpanConst const* larger = lhs_.size() < rhs_.size() ? &rhs_ : &lhs_;

  for (size_t i = 0; i < smaller->size(); ++i) {
    bitset_[i] = (*smaller)[i] & (*larger)[i];
  }
 }

auto encode_symbol(pmt::util::smrt::SymbolValueType symbol_) -> pmt::util::smrt::SymbolValueType {
 return (symbol_ == pmt::util::smrt::SymbolValueMax) ? 0 : symbol_ + 1;
}

auto decode_symbol(pmt::util::smrt::SymbolValueType symbol_) -> pmt::util::smrt::SymbolValueType {
 return (symbol_ == 0) ? pmt::util::smrt::SymbolValueMax : symbol_ - 1;
}

auto lowers_lt(pmt::util::smrt::SymbolValueType lhs_, pmt::util::smrt::SymbolValueType rhs_) -> bool {
 return decode_symbol(lhs_) < decode_symbol(rhs_);
}

auto encode_accept_index(pmt::util::smrt::AcceptsIndexType index_) -> pmt::util::smrt::AcceptsIndexType {
 return index_ + 1;
}

auto decode_accept_index(pmt::util::smrt::AcceptsIndexType index_) -> pmt::util::smrt::AcceptsIndexType {
 return index_ - 1;
}

auto get_state_nr_next_generic(std::ranges::random_access_range auto transitions_, std::ranges::random_access_range auto state_offsets_, std::ranges::random_access_range auto symbol_kind_offsets_, pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolKindType kind_, pmt::util::smrt::SymbolValueType symbol_) -> pmt::util::smrt::StateNrType {
 size_t const start = state_offsets_[symbol_kind_offsets_[kind_] + state_nr_];
 size_t const end = state_offsets_[symbol_kind_offsets_[kind_] + state_nr_ + 1];
 size_t const size = end - start;

 
 std::span<typename decltype(transitions_)::value_type> const lowers(&transitions_[start], size);
 
 size_t const uppers_offset = std::size(transitions_) / 3;
 std::span<typename decltype(transitions_)::value_type> const uppers(&transitions_[uppers_offset + start], size);

 size_t const values_offset = 2 * uppers_offset;
 std::span<typename decltype(transitions_)::value_type> const values(&transitions_[values_offset + start], size);

 size_t const idx = std::distance(lowers.begin(), std::lower_bound(lowers.begin(), lowers.end(), encode_symbol(symbol_), lowers_lt));

 if (idx == lowers.size()) {
  if (idx == 0 || symbol_ > decode_symbol(uppers[idx - 1])) {
   return pmt::util::smrt::StateNrSink;
  }
  return values[idx - 1];
 }

 if (symbol_ < decode_symbol(lowers[idx])) {
  if (idx == 0) {
   return pmt::util::smrt::StateNrSink;
  }
  if (symbol_ <= decode_symbol(uppers[idx - 1])) {
   return values[idx - 1];
  }
  return pmt::util::smrt::StateNrSink;
 }

 return values[idx];
}

}