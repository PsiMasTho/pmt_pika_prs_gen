#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <ranges>

namespace pmt::util::smrt {

inline auto find_first_set_bit(pmt::base::Bitset::ChunkSpanConst const& bitset_) -> size_t;

inline void set_bit(pmt::base::Bitset::ChunkSpan bitset_, size_t index_);

inline auto get_bit(pmt::base::Bitset::ChunkSpanConst bitset_, size_t index_) -> bool;

inline void bitwise_and(pmt::base::Bitset::ChunkSpan bitset_, pmt::base::Bitset::ChunkSpanConst lhs_ , pmt::base::Bitset::ChunkSpanConst rhs_);

inline auto encode_symbol(pmt::util::smrt::SymbolValueType symbol_) -> pmt::util::smrt::SymbolValueType;

inline auto decode_symbol(pmt::util::smrt::SymbolValueType symbol_) -> pmt::util::smrt::SymbolValueType;

inline auto lowers_lt(pmt::util::smrt::SymbolValueType lhs_, pmt::util::smrt::SymbolValueType rhs_) -> bool;

inline auto encode_accept_index(pmt::util::smrt::AcceptsIndexType index_) -> pmt::util::smrt::AcceptsIndexType;

inline auto decode_accept_index(pmt::util::smrt::AcceptsIndexType index_) -> pmt::util::smrt::AcceptsIndexType;

auto get_state_nr_next_generic(std::ranges::random_access_range auto transitions_, std::ranges::random_access_range auto state_offsets_, std::ranges::random_access_range auto symbol_kind_offsets_, pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolKindType kind_, pmt::util::smrt::SymbolValueType symbol_) -> pmt::util::smrt::StateNrType;

}

#include "pmt/util/smrt/util-inl.hpp"