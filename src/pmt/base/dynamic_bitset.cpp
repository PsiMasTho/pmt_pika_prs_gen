#include "pmt/base/dynamic_bitset.hpp"

#include "pmt/base/hash.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <numeric>
#include <utility>

namespace pmt::base {

DynamicBitset::DynamicBitset(size_t size_, bool value_)
 : _data(nullptr)
 , _size(size_)
 , _capacity(round_up_to_chunk_size(_size)) {
  if (_size == 0) {
    return;
  }

  _data = std::make_unique_for_overwrite<ChunkType[]>(get_required_chunk_count(_capacity));
  std::fill(_data.get(), _data.get() + get_required_chunk_count(_size), ALL_SET_MASKS[value_]);
  set_trailing_chunk(DEFAULT_VALUE);
}

DynamicBitset::DynamicBitset(const DynamicBitset& other_)
 : _data(nullptr)
 , _size(other_._size)
 , _capacity(round_up_to_chunk_size(_size)) {
  _data = std::make_unique_for_overwrite<ChunkType[]>(get_required_chunk_count(_capacity));
  std::copy(other_._data.get(), other_._data.get() + get_required_chunk_count(_size), _data.get());
  set_trailing_chunk(DEFAULT_VALUE);
}

DynamicBitset::DynamicBitset(DynamicBitset&& other_) noexcept
 : _data(std::move(other_._data))
 , _size(std::exchange(other_._size, 0))
 , _capacity(std::exchange(other_._capacity, 0)) {
}

auto DynamicBitset::operator=(const DynamicBitset& other_) -> DynamicBitset& {
  if (this == &other_) {
    return *this;
  }

  DynamicBitset tmp(other_);
  _data = std::move(tmp._data);
  _size = tmp._size;
  _capacity = tmp._capacity;
  return *this;
}

auto DynamicBitset::operator=(DynamicBitset&& other_) noexcept -> DynamicBitset& {
  if (this == &other_) {
    return *this;
  }

  DynamicBitset tmp(std::move(other_));
  _data = std::move(tmp._data);
  _size = tmp._size;
  _capacity = tmp._capacity;
  return *this;
}

auto DynamicBitset::size() const -> size_t {
  return _size;
}

auto DynamicBitset::capacity() const -> size_t {
  return _capacity;
}

auto DynamicBitset::empty() const -> bool {
  return _size == 0;
}

void DynamicBitset::clear() {
  _size = 0;
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::resize(size_t size_, bool value_) {
  if (_size < size_) {
    size_t const initial_chunks = get_required_chunk_count(_size);
    size_t const required_chunks = get_required_chunk_count(size_);
    reserve(required_chunks * CHUNK_BIT);
    set_trailing_chunk(value_);
    ChunkType* const start = _data.get() + initial_chunks;
    ChunkType* const end = _data.get() + required_chunks;
    if (start < end) {
      std::fill(start, end, ALL_SET_MASKS[value_]);
    }
  }
  _size = size_;
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::reserve(size_t capacity_) {
  capacity_ = round_up_to_chunk_size(capacity_);
  if (_capacity >= capacity_) {
    return;
  }

  std::unique_ptr<ChunkType[]> new_data = std::make_unique_for_overwrite<ChunkType[]>(capacity_);
  if (_data.get() != nullptr) {
    std::copy(_data.get(), _data.get() + get_required_chunk_count(_size), new_data.get());
  }
  _data = std::move(new_data);
  _capacity = capacity_;
}

void DynamicBitset::push_back(bool value_) {
  if (_size == _capacity) {
    reserve(get_next_capacity(_capacity));
  }

  set(_size++, value_);
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::pop_back() {
  if (_size == 0) {
    return;
  }
  set(_size - 1, DEFAULT_VALUE);
  --_size;
}

auto DynamicBitset::get(size_t index_) const -> bool {
  size_t const chunk_index = get_chunk_index(index_);
  size_t const bit_index = get_bit_index(index_);
  return (_data[chunk_index] & (ChunkType(1) << bit_index)) != 0;
}

auto DynamicBitset::set(size_t index_, bool value_) -> bool {
  assert(index_ < _size);

  size_t const chunk_index = get_chunk_index(index_);
  size_t const bit_index = get_bit_index(index_);
  ChunkType mask = ChunkType(1) << bit_index;
  bool const old_value = (_data[chunk_index] & mask) != 0;
  set_mask(_data[chunk_index], mask, value_);

  return old_value;
}

auto DynamicBitset::toggle(size_t index_) -> bool {
  assert(index_ < _size);

  size_t const chunk_index = get_chunk_index(index_);
  size_t const bit_index = get_bit_index(index_);
  ChunkType mask = ChunkType(1) << bit_index;
  bool const old_value = (_data[chunk_index] & mask) != 0;
  set_mask(_data[chunk_index], mask, !old_value);

  return old_value;
}

void DynamicBitset::set_all(bool value_) {
  ChunkType const mask = ALL_SET_MASKS[value_];
  std::fill(_data.get(), _data.get() + get_required_chunk_count(_size), mask);
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::toggle_all() {
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    _data[i] = ~_data[i];
  }
  set_trailing_chunk(DEFAULT_VALUE);
}

auto DynamicBitset::get_chunk(size_t index_) const -> ChunkType {
  assert(index_ < get_required_chunk_count(_size));
  return _data[index_];
}

auto DynamicBitset::get_chunk_count() const -> size_t {
  return get_required_chunk_count(_size);
}

auto DynamicBitset::get_required_chunk_count(size_t size_) -> size_t {
  return (size_ + CHUNK_BIT - 1) / CHUNK_BIT;
}

auto DynamicBitset::operator==(const DynamicBitset& other_) const -> bool {
  return _size == other_._size && std::equal(_data.get(), _data.get() + get_required_chunk_count(_size), other_._data.get());
}

auto DynamicBitset::operator!=(const DynamicBitset& other_) const -> bool {
  return !(*this == other_);
}

auto DynamicBitset::any() const -> bool {
  return std::any_of(_data.get(), _data.get() + get_required_chunk_count(_size), [](ChunkType chunk_) { return chunk_ != ALL_SET_MASKS[0]; });
}

auto DynamicBitset::none() const -> bool {
  return std::all_of(_data.get(), _data.get() + get_required_chunk_count(_size), [](ChunkType chunk_) { return chunk_ == ALL_SET_MASKS[0]; });
}

auto DynamicBitset::all() const -> bool {
  size_t max = get_required_chunk_count(_size) - 1;
  bool const lhs = std::all_of(_data.get(), _data.get() + max, [](ChunkType chunk_) { return chunk_ == ALL_SET_MASKS[1]; });
  if (!lhs) {
    return false;
  }
  return static_cast<size_t>(std::popcount(_data[max])) == get_bit_index(_size);
}

auto DynamicBitset::popcnt() const -> size_t {
  return std::accumulate(_data.get(), _data.get() + get_required_chunk_count(_size), 0, [](size_t acc_, ChunkType chunk_) { return acc_ + std::popcount(chunk_); });
}

auto DynamicBitset::countl(bool value_) const -> size_t {
  size_t const max = get_required_chunk_count(_size);
  auto const fn = value_ ? &std::countr_one<ChunkType> : &std::countr_zero<ChunkType>;

  size_t total = 0;
  for (size_t i = 0; i < max; ++i) {
    ChunkType chunk = _data[i];

    if (i == max - 1 && _size % CHUNK_BIT != 0) {
      set_mask(chunk, get_trailing_mask(_size), !value_);
    }

    size_t const incr = fn(chunk);
    total += incr;
    if (incr != CHUNK_BIT) {
      break;
    }
  }

  return total;
}

auto DynamicBitset::countr(bool value_) const -> size_t {
  size_t const max = get_required_chunk_count(_size) - 1;
  auto const fn = value_ ? &std::countl_one<ChunkType> : &std::countl_zero<ChunkType>;

  size_t total = 0;
  size_t i = max;
  while (true) {
    ChunkType chunk = _data[i];
    size_t breakout = CHUNK_BIT;
    if (i == max) {
      breakout = get_bit_index(_size);
      chunk <<= CHUNK_BIT - breakout;
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

void DynamicBitset::swap(size_t lhs_, size_t rhs_) {
  set(lhs_, exchange(rhs_, get(lhs_)));
}

auto DynamicBitset::exchange(size_t index_, bool value_) -> bool {
  bool const old_value = get(index_);
  set(index_, value_);
  return old_value;
}

void DynamicBitset::inplace_not() {
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    _data[i] = ~_data[i];
  }
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::inplace_or(const DynamicBitset& other_) {
  assert(_size == other_._size);
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    _data[i] |= other_._data[i];
  }
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::inplace_and(const DynamicBitset& other_) {
  assert(_size == other_._size);
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    _data[i] &= other_._data[i];
  }
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::inplace_xor(const DynamicBitset& other_) {
  assert(_size == other_._size);
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    _data[i] ^= other_._data[i];
  }
  set_trailing_chunk(DEFAULT_VALUE);
}

void DynamicBitset::inplace_nor(const DynamicBitset& other_) {
  assert(_size == other_._size);
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    _data[i] = ~(_data[i] | other_._data[i]);
  }
  set_trailing_chunk(DEFAULT_VALUE);
}

auto DynamicBitset::clone_not() const -> DynamicBitset {
  DynamicBitset ret;
  ret.reserve(_size);
  ret._size = _size;
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    ret._data[i] = ~_data[i];
  }
  ret.set_trailing_chunk(DEFAULT_VALUE);
  return ret;
}

auto DynamicBitset::clone_or(const DynamicBitset& other_) const -> DynamicBitset {
  assert(_size == other_._size);
  DynamicBitset ret;
  ret.reserve(_size);
  ret._size = _size;
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    ret._data[i] = _data[i] | other_._data[i];
  }
  ret.set_trailing_chunk(DEFAULT_VALUE);
  return ret;
}

auto DynamicBitset::clone_and(const DynamicBitset& other_) const -> DynamicBitset {
  assert(_size == other_._size);
  DynamicBitset ret;
  ret.reserve(_size);
  ret._size = _size;
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    ret._data[i] = _data[i] & other_._data[i];
  }
  ret.set_trailing_chunk(DEFAULT_VALUE);
  return ret;
}

auto DynamicBitset::clone_xor(const DynamicBitset& other_) const -> DynamicBitset {
  assert(_size == other_._size);
  DynamicBitset ret;
  ret.reserve(_size);
  ret._size = _size;
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    ret._data[i] = _data[i] ^ other_._data[i];
  }
  ret.set_trailing_chunk(DEFAULT_VALUE);
  return ret;
}

auto DynamicBitset::clone_nor(const DynamicBitset& other_) const -> DynamicBitset {
  assert(_size == other_._size);
  DynamicBitset ret;
  ret.reserve(_size);
  ret._size = _size;
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    ret._data[i] = ~(_data[i] | other_._data[i]);
  }
  ret.set_trailing_chunk(DEFAULT_VALUE);
  return ret;
}

void DynamicBitset::inplace_assymetric_difference(const DynamicBitset& other_) {
  assert(_size == other_._size);
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    _data[i] = ~(~_data[i] | other_._data[i]);
  }
  set_trailing_chunk(DEFAULT_VALUE);
}

auto DynamicBitset::clone_asymmetric_difference(const DynamicBitset& other_) const -> DynamicBitset {
  assert(_size == other_._size);
  DynamicBitset ret;
  ret.reserve(_size);
  ret._size = _size;
  for (size_t i = 0; i < get_required_chunk_count(_size); ++i) {
    ret._data[i] = ~(~_data[i] | other_._data[i]);
  }
  ret.set_trailing_chunk(DEFAULT_VALUE);
  return ret;
}

void DynamicBitset::set_trailing_chunk(bool value_) {
  if (_size % CHUNK_BIT == 0) {
    return;
  }

  size_t const trailing_mask = get_trailing_mask(_size);

  set_mask(_data[get_chunk_index(_size)], trailing_mask, value_);
}

auto DynamicBitset::get_bit_index(size_t index_) -> size_t {
  return index_ % CHUNK_BIT;
}

auto DynamicBitset::get_chunk_index(size_t index_) -> size_t {
  return index_ / CHUNK_BIT;
}

auto DynamicBitset::get_trailing_mask(size_t size_) -> ChunkType {
  return ~ChunkType(0) << get_bit_index(size_);
}

void DynamicBitset::set_mask(ChunkType& dest_, ChunkType mask_, bool value_) {
  if (value_) {
    dest_ |= mask_;
  } else {
    dest_ &= ~mask_;
  }
}

auto DynamicBitset::round_up_to_chunk_size(size_t size_) -> size_t {
  return (size_ + CHUNK_BIT - 1) / CHUNK_BIT * CHUNK_BIT;
}

auto DynamicBitset::get_next_capacity(size_t capacity_) -> size_t {
  static size_t const FIRST_CAPACITY = std::max(INITIAL_CAPACITY, size_t(1));
  static size_t const LAST_CAPACITY = std::numeric_limits<size_t>::max();

  if (capacity_ < FIRST_CAPACITY) {
    return FIRST_CAPACITY;
  }

  size_t const scaled_capacity = capacity_ * GrowthRate::type::num;

  // Overflow check
  if (scaled_capacity / GrowthRate::type::num != capacity_) {
    return LAST_CAPACITY;
  }

  return scaled_capacity / GrowthRate::type::den;
}

}  // namespace pmt::base

namespace std {
auto hash<pmt::base::DynamicBitset>::operator()(const pmt::base::DynamicBitset& bitset_) const -> size_t {
  size_t seed = 0;
  for (size_t i = 0; i < bitset_.get_required_chunk_count(bitset_.size()); ++i) {
    pmt::base::Hash::combine(bitset_._data.get()[i], seed);
  }
  return seed;
}
}  // namespace std