#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/base/interval_set.hpp"

#include <concepts>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace pmt::base {

class BitsetConverter {
public:
 // -$ Functions $-
 // --$ Other $--
 // --- String Conversion ---
 static auto to_string(Bitset const& bitset_) -> std::string;
 static auto from_string(std::string const& str_) -> Bitset;

 // --- Set Conversion ---
 template <std::integral T_ = size_t>
 static auto to_set(Bitset const& bitset_) -> std::set<T_>;
 template <std::integral T_ = size_t>
 static auto from_set(std::set<T_> const& set_) -> Bitset;

 // --- Unordered Set Conversion ---
 template <std::integral T_ = size_t>
 static auto to_unordered_set(Bitset const& bitset_) -> std::unordered_set<T_>;
 template <std::integral T_ = size_t>
 static auto from_unordered_set(std::unordered_set<T_> const& set_) -> Bitset;

 // --- Interval Set Conversion ---
 template <std::integral T_ = size_t>
 static auto to_interval_set(Bitset const& bitset_) -> IntervalSet<T_>;
 template <std::integral T_ = size_t>
 static auto from_interval_set(IntervalSet<T_> const& interval_set_) -> Bitset;
};

class BitsetConverterError : public std::logic_error {
public:
 BitsetConverterError();
};

}  // namespace pmt::base

#include "pmt/base/bitset_converter-inl.hpp"