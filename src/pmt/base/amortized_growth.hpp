#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <stdexcept>

namespace pmt::base {

class AmortizedGrowth {
public:
 // -$ Types / Constants $-
 enum : size_t {
  MaxCapacityIdx = 63,
  MaxCapacityIdxBitWidth = std::bit_width(MaxCapacityIdx),
  MaxCapacityBitWidth = 42
 };

 // -$ Functions $-
 // --$ Other $--
 [[nodiscard]] static auto idx_to_size(size_t idx_) -> uint64_t;
 [[nodiscard]] static auto size_to_idx(size_t size_, size_t lskip_idx_ = 0) -> size_t;

private:
 static std::array<uint64_t, MaxCapacityIdx + 1> const LUT;
};

class AmortizedGrowthError : public std::logic_error {
public:
 AmortizedGrowthError();
};

}  // namespace pmt::base
