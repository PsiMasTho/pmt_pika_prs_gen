#include "pmt/base/bitset.hpp"

#include "pmt/base/algo.hpp"
#include "pmt/base/hash.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <numeric>
#include <utility>

namespace pmt::base {

Bitset::Bitset()
 : Bitset(0) {
}

Bitset::Bitset(size_t size_, bool value_)
 : _data(nullptr)
 , _size(0)
 , _capacity_idx(0) {
 if (size_ == 0) {
  return;
 }

 reserve(size_);
 _size = size_;
 std::fill(_data.get(), _data.get() + get_required_chunk_count(_size), ALL_SET_MASKS[value_]);
 set_trailing_bits(DEFAULT_VALUE);
}

Bitset::Bitset(ChunkSpanConst span_)
 : _data(nullptr)
 , _size(0)
 , _capacity_idx(0) {
 if (span_.empty()) {
  return;
 }

 size_t const size = span_.size() * ChunkBit;
 reserve(size);
 _size = size;
 std::copy(span_.begin(), span_.end(), _data.get());
 // Note: There should be no trailing bits in the span
 // set_trailing_bits(DEFAULT_VALUE);
}

Bitset::Bitset(Bitset const& other_)
 : _data(nullptr)
 , _size(other_._size)
 , _capacity_idx(0) {
 reserve(other_._size);
 std::copy(other_._data.get(), other_._data.get() + get_required_chunk_count(_size), _data.get());
 set_trailing_bits(DEFAULT_VALUE);
}

auto Bitset::operator=(Bitset const& other_) -> Bitset& {
 if (this == &other_) {
  return *this;
 }

 Bitset tmp(other_);
 std::swap(_data, tmp._data);

 // swap bitfields manually because std::swap doesnt work
 size_t swap_tmp = _size;
 _size = tmp._size;
 tmp._size = swap_tmp;

 swap_tmp = _capacity_idx;
 _capacity_idx = tmp._capacity_idx;
 tmp._capacity_idx = swap_tmp;

 return *this;
}

auto Bitset::operator==(Bitset const& other_) const -> bool {
 return _size == other_._size && std::equal(_data.get(), _data.get() + get_required_chunk_count(_size), other_._data.get());
}

auto Bitset::operator!=(Bitset const& other_) const -> bool {
 return !(*this == other_);
}

auto Bitset::hash() const -> size_t {
 size_t seed = Hash::Phi64;
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  Hash::combine(_data.get()[i], seed);
 }
 return seed;
}

auto Bitset::size() const -> size_t {
 return _size;
}

auto Bitset::capacity() const -> size_t {
 return _data ? AmortizedGrowth::idx_to_size(_capacity_idx) * ChunkBit : 0;
}

auto Bitset::empty() const -> bool {
 return _size == 0;
}

void Bitset::clear() {
 _size = 0;
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::resize(size_t size_, bool value_) {
 if (_size < size_) {
  size_t const initial_chunks = get_required_chunk_count(_size);
  size_t const required_chunks = get_required_chunk_count(size_);
  reserve(required_chunks * ChunkBit);
  set_trailing_bits(value_);
  ChunkType* const start = _data.get() + initial_chunks;
  ChunkType* const end = _data.get() + required_chunks;
  if (start < end) {
   std::fill(start, end, ALL_SET_MASKS[value_]);
  }
 }
 _size = size_;
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::reserve(size_t new_capacity_) {
 new_capacity_ = std::max(new_capacity_, size());

 size_t const new_capacity_idx = round_up_to_capacity_idx(new_capacity_);

 if (_data && new_capacity_idx == _capacity_idx) {
  return;
 }

 new_capacity_ = AmortizedGrowth::idx_to_size(new_capacity_idx) * ChunkBit;
 size_t const old_capacity = capacity();
 _capacity_idx = new_capacity_idx;
 realloc_unique_ptr(_data, get_required_chunk_count(old_capacity), get_required_chunk_count(new_capacity_));
}

void Bitset::push_back(bool value_) {
 if (size() == capacity()) {
  reserve(size() + 1);
 }

 set(_size++, value_);
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::pop_back() {
 if (_size == 0) {
  return;
 }
 set(_size - 1, DEFAULT_VALUE);
 --_size;
}

auto Bitset::get(size_t index_) const -> bool {
 assert(index_ < _size);
 size_t const chunk_index = get_chunk_index(index_);
 size_t const bit_index = get_bit_index(index_);
 return (_data[chunk_index] & (ChunkType(1) << bit_index)) != 0;
}

auto Bitset::set(size_t index_, bool value_) -> bool {
 assert(index_ < _size);

 size_t const chunk_index = get_chunk_index(index_);
 size_t const bit_index = get_bit_index(index_);
 ChunkType mask = ChunkType(1) << bit_index;
 bool const old_value = (_data[chunk_index] & mask) != 0;
 set_mask(_data[chunk_index], mask, value_);

 return old_value;
}

auto Bitset::toggle(size_t index_) -> bool {
 assert(index_ < _size);

 size_t const chunk_index = get_chunk_index(index_);
 size_t const bit_index = get_bit_index(index_);
 ChunkType mask = ChunkType(1) << bit_index;
 bool const old_value = (_data[chunk_index] & mask) != 0;
 set_mask(_data[chunk_index], mask, !old_value);

 return old_value;
}

void Bitset::set_all(bool value_) {
 ChunkType const mask = ALL_SET_MASKS[value_];
 std::fill(_data.get(), _data.get() + get_required_chunk_count(_size), mask);
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::toggle_all() {
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  _data[i] = ~_data[i];
 }
 set_trailing_bits(DEFAULT_VALUE);
}

auto Bitset::get_chunks() const -> ChunkSpanConst {
 return std::span<ChunkType const>(_data.get(), get_required_chunk_count(_size));
}

auto Bitset::get_chunk(size_t index_) const -> ChunkType {
 assert(index_ < get_required_chunk_count(_size));
 return _data[index_];
}

auto Bitset::get_chunk_count() const -> size_t {
 return get_required_chunk_count(_size);
}

auto Bitset::get_required_chunk_count(size_t size_) -> size_t {
 return (size_ + ChunkBit - 1) / ChunkBit;
}

auto Bitset::front() const -> bool {
 assert(_size > 0);
 return get(0);
}

auto Bitset::back() const -> bool {
 assert(_size > 0);
 return get(_size - 1);
}

auto Bitset::any() const -> bool {
 return std::any_of(_data.get(), _data.get() + get_required_chunk_count(_size), [](ChunkType chunk_) { return chunk_ != ALL_SET_MASKS[0]; });
}

auto Bitset::none() const -> bool {
 return std::all_of(_data.get(), _data.get() + get_required_chunk_count(_size), [](ChunkType chunk_) { return chunk_ == ALL_SET_MASKS[0]; });
}

auto Bitset::all() const -> bool {
 if (_size == 0) {
  return true;
 }

 size_t max = get_required_chunk_count(_size) - 1;
 bool const lhs = std::all_of(_data.get(), _data.get() + max, [](ChunkType chunk_) { return chunk_ == ALL_SET_MASKS[1]; });
 if (!lhs) {
  return false;
 }
 return static_cast<size_t>(std::popcount(_data[max])) == get_bit_index(_size);
}

auto Bitset::popcnt() const -> size_t {
 if (_size == 0) {
  return 0;
 }
 return std::accumulate(_data.get(), _data.get() + get_required_chunk_count(_size), 0, [](size_t acc_, ChunkType chunk_) { return acc_ + std::popcount(chunk_); });
}

auto Bitset::countl(bool value_) const -> size_t {
 size_t const max = get_required_chunk_count(_size);
 auto const fn = value_ ? &std::countr_one<ChunkType> : &std::countr_zero<ChunkType>;

 size_t total = 0;
 for (size_t i = 0; i < max; ++i) {
  ChunkType chunk = _data[i];

  if (i == max - 1 && _size % ChunkBit != 0) {
   set_mask(chunk, get_trailing_mask(_size), !value_);
  }

  size_t const incr = fn(chunk);
  total += incr;
  if (incr != ChunkBit) {
   break;
  }
 }

 return total;
}

auto Bitset::countr(bool value_) const -> size_t {
 size_t const required_chunk_count = get_required_chunk_count(_size);
 if (required_chunk_count == 0) {
  return 0;
 }

 size_t const max = required_chunk_count - 1;
 auto const fn = value_ ? &std::countl_one<ChunkType> : &std::countl_zero<ChunkType>;

 size_t total = 0;
 size_t i = max;
 while (true) {
  ChunkType chunk = _data[i];
  size_t breakout = ChunkBit;
  if (i == max) {
   breakout = get_bit_index(_size);
   chunk <<= ChunkBit - breakout;
  }

  size_t const incr = std::min(breakout, static_cast<size_t>(fn(chunk)));
  total += incr;
  if (i == 0 || incr != breakout) {
   break;
  }
  --i;
 }

 return total;
}

void Bitset::swap(size_t lhs_, size_t rhs_) {
 set(lhs_, exchange(rhs_, get(lhs_)));
}

auto Bitset::exchange(size_t index_, bool value_) -> bool {
 bool const old_value = get(index_);
 set(index_, value_);
 return old_value;
}

void Bitset::inplace_not() {
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  _data[i] = ~_data[i];
 }
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::inplace_or(Bitset const& other_) {
 assert(_size == other_._size);
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  _data[i] |= other_._data[i];
 }
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::inplace_and(Bitset const& other_) {
 assert(_size == other_._size);
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  _data[i] &= other_._data[i];
 }
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::inplace_xor(Bitset const& other_) {
 assert(_size == other_._size);
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  _data[i] ^= other_._data[i];
 }
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::inplace_nor(Bitset const& other_) {
 assert(_size == other_._size);
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  _data[i] = ~(_data[i] | other_._data[i]);
 }
 set_trailing_bits(DEFAULT_VALUE);
}

void Bitset::inplace_assymetric_difference(Bitset const& other_) {
 assert(_size == other_._size);
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  _data[i] = ~(~_data[i] | other_._data[i]);
 }
 set_trailing_bits(DEFAULT_VALUE);
}

auto Bitset::clone_not() const -> Bitset {
 Bitset ret;
 ret.reserve(_size);
 ret._size = _size;
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  ret._data[i] = ~_data[i];
 }
 ret.set_trailing_bits(DEFAULT_VALUE);
 return ret;
}

auto Bitset::clone_or(Bitset const& other_) const -> Bitset {
 assert(_size == other_._size);
 Bitset ret;
 ret.reserve(_size);
 ret._size = _size;
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  ret._data[i] = _data[i] | other_._data[i];
 }
 ret.set_trailing_bits(DEFAULT_VALUE);
 return ret;
}

auto Bitset::clone_and(Bitset const& other_) const -> Bitset {
 assert(_size == other_._size);
 Bitset ret;
 ret.reserve(_size);
 ret._size = _size;
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  ret._data[i] = _data[i] & other_._data[i];
 }
 ret.set_trailing_bits(DEFAULT_VALUE);
 return ret;
}

auto Bitset::clone_xor(Bitset const& other_) const -> Bitset {
 assert(_size == other_._size);
 Bitset ret;
 ret.reserve(_size);
 ret._size = _size;
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  ret._data[i] = _data[i] ^ other_._data[i];
 }
 ret.set_trailing_bits(DEFAULT_VALUE);
 return ret;
}

auto Bitset::clone_nor(Bitset const& other_) const -> Bitset {
 assert(_size == other_._size);
 Bitset ret;
 ret.reserve(_size);
 ret._size = _size;
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  ret._data[i] = ~(_data[i] | other_._data[i]);
 }
 ret.set_trailing_bits(DEFAULT_VALUE);
 return ret;
}

auto Bitset::clone_asymmetric_difference(Bitset const& other_) const -> Bitset {
 assert(_size == other_._size);
 Bitset ret;
 ret.reserve(_size);
 ret._size = _size;
 for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
  ret._data[i] = ~(~_data[i] | other_._data[i]);
 }
 ret.set_trailing_bits(DEFAULT_VALUE);
 return ret;
}

void Bitset::set_trailing_bits(bool value_) {
 if (_size % ChunkBit == 0) {
  return;
 }

 size_t const trailing_mask = get_trailing_mask(_size);

 set_mask(_data[get_chunk_index(_size)], trailing_mask, value_);
}

auto Bitset::get_bit_index(size_t index_) -> size_t {
 return index_ % ChunkBit;
}

auto Bitset::get_chunk_index(size_t index_) -> size_t {
 return index_ / ChunkBit;
}

auto Bitset::get_trailing_mask(size_t size_) -> ChunkType {
 return ~ChunkType(0) << get_bit_index(size_);
}

void Bitset::set_mask(ChunkType& dest_, ChunkType mask_, bool value_) {
 if (value_) {
  dest_ |= mask_;
 } else {
  dest_ &= ~mask_;
 }
}

auto Bitset::round_up_to_capacity_idx(size_t size_) -> size_t {
 size_ += ChunkBit - 1;
 size_ /= ChunkBit;
 return AmortizedGrowth::size_to_idx(size_);
}

}  // namespace pmt::base