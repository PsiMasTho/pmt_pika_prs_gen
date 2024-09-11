#ifndef PMT_BASE_HASH_H
#define PMT_BASE_HASH_H

#include <stddef.h>

size_t
pmt_base_hash_fnv1a(void const* data_, size_t size_);

size_t
pmt_base_hash_combine(size_t seed_, size_t hash_);

#endif //PMT_BASE_HASH_H
