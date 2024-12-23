#pragma once

#include <array>
#include <functional>
#include <memory>
#include <ratio>

namespace pmt::base {

class DynamicBitset {
 public:
  // - Public types / constants -
  using ChunkType = std::uint64_t;
  static constexpr size_t CHUNK_BIT = sizeof(ChunkType) * CHAR_BIT;

  // - Lifetime -
  explicit DynamicBitset(size_t size_ = 0, bool value_ = false);
  DynamicBitset(const DynamicBitset& other_);
  DynamicBitset(DynamicBitset&& other_) noexcept;

  auto operator=(const DynamicBitset& other_) -> DynamicBitset&;
  auto operator=(DynamicBitset&& other_) noexcept -> DynamicBitset&;

  ~DynamicBitset() = default;

  // - Capacity -
  auto size() const -> size_t;
  auto capacity() const -> size_t;
  auto empty() const -> bool;
  void clear();
  void resize(size_t size_, bool value_ = false);
  void reserve(size_t capacity_);
  void push_back(bool value_);
  void pop_back();

  // - Single bit access -
  auto get(size_t index_) const -> bool;
  auto set(size_t index_, bool value_) -> bool;
  auto toggle(size_t index_) -> bool;

  // - All bit access -
  void set_all(bool value_);
  void toggle_all();

  // - Chunk access -
  auto get_chunk(size_t index_) const -> ChunkType;
  auto get_chunk_count() const -> size_t;
  static auto get_required_chunk_count(size_t size_) -> size_t;

  // - Equality -
  auto operator==(const DynamicBitset& other_) const -> bool;
  auto operator!=(const DynamicBitset& other_) const -> bool;

  // - Bitwise operations -
  auto any() const -> bool;
  auto none() const -> bool;
  auto all() const -> bool;
  auto popcnt() const -> size_t;
  auto countl(bool value_) const -> size_t;
  auto countr(bool value_) const -> size_t;
  void swap(size_t lhs_, size_t rhs_);
  auto exchange(size_t index_, bool value_) -> bool;

  // -- Inplace --
  void inplace_not();
  void inplace_or(const DynamicBitset& other_);   // Union
  void inplace_and(const DynamicBitset& other_);  // Intersection
  void inplace_xor(const DynamicBitset& other_);  // Symmetric difference
  void inplace_nor(const DynamicBitset& other_);

  // -- Cloning --
  auto clone_not() const -> DynamicBitset;
  auto clone_or(const DynamicBitset& other_) const -> DynamicBitset;   // Union
  auto clone_and(const DynamicBitset& other_) const -> DynamicBitset;  // Intersection
  auto clone_xor(const DynamicBitset& other_) const -> DynamicBitset;  // Symmetric difference
  auto clone_nor(const DynamicBitset& other_) const -> DynamicBitset;

  // - Set operations -
  // -- Inplace --
  void inplace_assymetric_difference(const DynamicBitset& other_);  // in this but not in other

  // -- Cloning --
  auto clone_asymmetric_difference(const DynamicBitset& other_) const -> DynamicBitset;  // in this but not in other

 private:
  // - Friends -
  friend struct std::hash<DynamicBitset>;

  // - Private types / constants -
  using GrowthRate = std::ratio<3, 2>;
  static constexpr size_t INITIAL_CAPACITY = CHUNK_BIT;
  static constexpr bool DEFAULT_VALUE = false;
  static constexpr std::array<ChunkType, 2> ALL_SET_MASKS = {ChunkType(0), ~ChunkType(0)};

  // - Private functions -
  void set_trailing_chunk(bool value_);

  // - Private static functions -
  static auto get_bit_index(size_t index_) -> size_t;
  static auto get_chunk_index(size_t index_) -> size_t;
  static auto get_trailing_mask(size_t size_) -> ChunkType;
  static void set_mask(ChunkType& dest_, ChunkType mask_, bool value_);
  static auto round_up_to_chunk_size(size_t size_) -> size_t;
  static auto get_next_capacity(size_t capacity_) -> size_t;

  // -- Data --
  // Implementation note:
  // The chunk of _size is filled with DEFAULT_VALUE after the last bit.
  // The chunks after that up to _capacity are of unspecified value.
  std::unique_ptr<ChunkType[]> _data;
  size_t _size;      // in bits
  size_t _capacity;  // in bits
};
}  // namespace pmt::base

namespace std {
template <>
struct hash<pmt::base::DynamicBitset> {
  auto operator()(const pmt::base::DynamicBitset& bitset_) const -> size_t;
};
}  // namespace std