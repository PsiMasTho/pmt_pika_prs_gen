#include "pmt/base/hash.hpp"

namespace pmt::base {

namespace {
enum : size_t {
  FnvOffset = 14695981039346656037ull,
  FnvPrime = 1099511628211ull,
  CombineConstant = 0x9e3779b9ull
};

}  // namespace

auto Hash::fnv1a(unsigned char const* data_, size_t size_) -> size_t {
  size_t hash = FnvOffset;

  unsigned char const* const end = data_ + size_;
  while (data_ != end)
    hash = (hash ^ *data_++) * FnvPrime;

  return hash;
}

void Hash::combine(size_t hash_, size_t& seed_) {
  seed_ ^= (hash_ + CombineConstant + (seed_ << 6) + (seed_ >> 2));
}

}  // namespace pmt::base