#include "pmt/container/amortized_growth.hpp"

#include <algorithm>

namespace pmt::container {

auto AmortizedGrowth::idx_to_size(size_t idx_) -> uint64_t {
 return LUT[idx_];
}

auto AmortizedGrowth::size_to_idx(size_t size_, size_t lskip_idx_) -> size_t {
 auto const itr = std::lower_bound(LUT.begin() + lskip_idx_, LUT.end(), size_);
 if (itr == LUT.end()) {
  throw AmortizedGrowthError{};
 }

 return std::distance(LUT.begin(), itr);
}

// clang-format off
std::array<uint64_t, AmortizedGrowth::MaxCapacityIdx + 1> const AmortizedGrowth::LUT = {
  /* x2 */
  1ULL,    2ULL,    4ULL,    8ULL,    16ULL,   32ULL,   64ULL,
  128ULL,  256ULL,  512ULL,  1024ULL, 2048ULL, 4096ULL,
  /* x1.5 */
  6144ULL,          9216ULL,          13824ULL,          20736ULL,          31104ULL,
  46656ULL,         69984ULL,         104976ULL,         157464ULL,         236196ULL,
  354294ULL,        531441ULL,        797161ULL,         1195742ULL,        1793613ULL,
  2690420ULL,       4035630ULL,       6053445ULL,        9080167ULL,        13620251ULL,
  20430377ULL,      30645566ULL,      45968349ULL,       68952523ULL,       103428785ULL,
  155143177ULL,     232714766ULL,     349072150ULL,      523608225ULL,      785412338ULL,
  1178118507ULL,    1767177761ULL,    2650766642ULL,     3976149964ULL,     5964224946ULL,
  8946337419ULL,    13419506129ULL,   20129259194ULL,    30193888791ULL,    45290833186ULL,
  67936249779ULL,   101904374669ULL,  152856562004ULL,   229284843007ULL,   343927264510ULL,
  515890896765ULL,  773836345148ULL,  1160754517722ULL,  1741131776584ULL,  2611697664876ULL,
  3917546497315ULL
 };
// clang-format on

AmortizedGrowthError::AmortizedGrowthError()
 : std::logic_error{"capacity limit exceeded"} {
}
}  // namespace pmt::container