#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/sm/primitives.hpp"

#include <ranges>

namespace pmt::parser::rt {

inline auto find_first_set_bit(pmt::base::Bitset::ChunkSpanConst const& bitset_) -> size_t;

inline void set_bit(pmt::base::Bitset::ChunkSpan bitset_, size_t index_);

inline void clear_bit(pmt::base::Bitset::ChunkSpan bitset_, size_t index_);

inline auto get_bit(pmt::base::Bitset::ChunkSpanConst bitset_, size_t index_) -> bool;

inline auto get_popcount(pmt::base::Bitset::ChunkSpanConst bitset_) -> size_t;

inline void bitwise_and(pmt::base::Bitset::ChunkSpan bitset_, pmt::base::Bitset::ChunkSpanConst lhs_, pmt::base::Bitset::ChunkSpanConst rhs_);

inline auto encode_symbol(pmt::sm::SymbolType symbol_) -> pmt::sm::SymbolType;

inline auto decode_symbol(pmt::sm::SymbolType symbol_) -> pmt::sm::SymbolType;

inline auto lowers_lt(pmt::sm::SymbolType lhs_, pmt::sm::SymbolType rhs_) -> bool;

inline auto encode_accept_index(pmt::sm::AcceptsIndexType index_) -> pmt::sm::AcceptsIndexType;

inline auto decode_accept_index(pmt::sm::AcceptsIndexType index_) -> pmt::sm::AcceptsIndexType;

auto get_state_nr_next_generic(std::ranges::random_access_range auto transitions_, std::ranges::random_access_range auto state_offsets_, std::ranges::random_access_range auto symbol_kind_offsets_, pmt::sm::StateNrType state_nr_, SymbolKindType kind_, pmt::sm::SymbolType symbol_) -> pmt::sm::StateNrType;

}  // namespace pmt::parser::rt

#include "pmt/parser/rt/util-inl.hpp"