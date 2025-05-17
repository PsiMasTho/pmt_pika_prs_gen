#pragma once

#include "pmt/base/amortized_growth.hpp"
#include "pmt/base/hashable.hpp"

#include <array>
#include <memory>
#include <span>

namespace pmt::base {

class Bitset : public Hashable<Bitset> {
 public:
  // -$ Types / Constants $-
  using ChunkType = uint64_t;
  using ChunkSpan = std::span<ChunkType>;
  using ChunkSpanConst = std::span<ChunkType const>;

  enum : size_t {
    ChunkBit = sizeof(ChunkType) * CHAR_BIT,
  };

 static constexpr std::array<ChunkType, 2> ALL_SET_MASKS = {ChunkType(0), ~ChunkType(0)};

 private:
  static constexpr bool DEFAULT_VALUE = false;

 private:
  // -$ Data $-
  std::unique_ptr<ChunkType[]> _data;
  size_t _size : AmortizedGrowth::MaxCapacityBitWidth = 0;
  size_t _capacity_idx : AmortizedGrowth::MaxCapacityIdxBitWidth = 0;

 public:
  // -$ Functions $-
  // --$ Lifetime $--
  Bitset();
  explicit Bitset(size_t size_, bool value_ = false);
  explicit Bitset(ChunkSpanConst span_);
  Bitset(Bitset const& other_);
  Bitset(Bitset&&) noexcept = default;
  ~Bitset() = default;

  // --$ Operators $--
  auto operator=(Bitset const& other_) -> Bitset&;
  auto operator=(Bitset&&) noexcept -> Bitset& = default;
  auto operator==(Bitset const& other_) const -> bool;
  auto operator!=(Bitset const& other_) const -> bool;

  // --$ Inherited: pmt::base::Hashable $--
  auto hash() const -> size_t;

  // --$ Other $--
  auto size() const -> size_t;
  auto capacity() const -> size_t;
  auto empty() const -> bool;
  void clear();
  void resize(size_t size_, bool value_ = false);
  void reserve(size_t new_capacity_);
  void push_back(bool value_);
  void pop_back();

  auto get(size_t index_) const -> bool;
  auto set(size_t index_, bool value_) -> bool;
  auto toggle(size_t index_) -> bool;

  void set_all(bool value_);
  void toggle_all();

  auto get_chunks() const -> ChunkSpanConst;
  auto get_chunk(size_t index_) const -> ChunkType;
  auto get_chunk_count() const -> size_t;
  static auto get_required_chunk_count(size_t size_) -> size_t;

  auto front() const -> bool;
  auto back() const -> bool;
  auto any() const -> bool;
  auto none() const -> bool;
  auto all() const -> bool;
  auto popcnt() const -> size_t;
  auto countl(bool value_) const -> size_t;
  auto countr(bool value_) const -> size_t;
  void swap(size_t lhs_, size_t rhs_);
  auto exchange(size_t index_, bool value_) -> bool;

  void inplace_not();
  void inplace_or(Bitset const& other_);   // Union
  void inplace_and(Bitset const& other_);  // Intersection
  void inplace_xor(Bitset const& other_);  // Symmetric difference
  void inplace_nor(Bitset const& other_);
  void inplace_assymetric_difference(Bitset const& other_);  // in this but not in other

  auto clone_not() const -> Bitset;
  auto clone_or(Bitset const& other_) const -> Bitset;   // Union
  auto clone_and(Bitset const& other_) const -> Bitset;  // Intersection
  auto clone_xor(Bitset const& other_) const -> Bitset;  // Symmetric difference
  auto clone_nor(Bitset const& other_) const -> Bitset;
  auto clone_asymmetric_difference(Bitset const& other_) const -> Bitset;  // in this but not in other

 private:
  void set_trailing_bits(bool value_);

  static auto get_bit_index(size_t index_) -> size_t;
  static auto get_chunk_index(size_t index_) -> size_t;
  static auto get_trailing_mask(size_t size_) -> ChunkType;
  static void set_mask(ChunkType& dest_, ChunkType mask_, bool value_);
  static auto round_up_to_capacity_idx(size_t size_) -> size_t;
};
}  // namespace pmt::base
