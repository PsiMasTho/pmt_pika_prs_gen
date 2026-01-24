#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace pmt::util {

static inline char const hex_alphabet_uppercase[] = "0123456789ABCDEF";

// if width is less than required, no padding is added.
// if padding is added, it uses the character at alphabet_[0]
// alphabet_.size() is the base used
auto uint_to_string(uint64_t value_, size_t width_, std::string_view alphabet_) -> std::string;

auto digits_needed(uint64_t value_, uint64_t base_) -> size_t;

}  // namespace pmt::util