#include "pmt/base/hash.hpp"

namespace pmt::base {

namespace {
enum : size_t {
  FNV_OFFSET = 14695981039346656037ull,
  FNV_PRIME = 1099511628211ull,
  COMBINE_CONSTANT = 0x9e3779b9ull
};

}  // namespace

auto hash::fnv1a(unsigned char const* data_, size_t size_) -> size_t {
  size_t hash = FNV_OFFSET;

  unsigned char const* const end = data_ + size_;
  while (data_ != end)
    hash = (hash ^ *data_++) * FNV_PRIME;

  return hash;
}

void hash::combine(size_t hash_, size_t& seed_) {
  seed_ ^= (hash_ + COMBINE_CONSTANT + (seed_ << 6) + (seed_ >> 2));
}

}  // namespace pmt::base