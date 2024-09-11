#include "pmt/base/hash.h"

#define FNV_OFFSET 14695981039346656037ull
#define FNV_PRIME 1099511628211ull

size_t
pmt_base_hash_fnv1a(void const* data_, size_t size_) {
 size_t hash = FNV_OFFSET;

 unsigned char const*       p   = data_;
 unsigned char const* const end = p + size_;
 while (p != end)
  hash = (hash ^ *p++) * FNV_PRIME;

 return hash;
}

size_t
pmt_base_hash_combine(size_t seed_, size_t hash_) {
 return seed_ ^ (hash_ + 0x9e3779b9ull + (seed_ << 6) + (seed_ >> 2));
}
