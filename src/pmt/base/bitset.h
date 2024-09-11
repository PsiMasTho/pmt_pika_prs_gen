#ifndef PMT_BASE_BITSET_H
#define PMT_BASE_BITSET_H

#include "pmt/base/opaque_handle.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct pmt_base_opaque_handle_2 pmt_base_bitset;

pmt_base_bitset
pmt_base_bitset_create(size_t size_, bool fill_);

void
pmt_base_bitset_destroy(void* self_);

bool
pmt_base_bitset_at(pmt_base_bitset const* self_, size_t pos_);

size_t
pmt_base_bitset_size(pmt_base_bitset const* self_);

void
pmt_base_bitset_set(pmt_base_bitset* self_, size_t pos_, bool value_);

void
pmt_base_bitset_resize(pmt_base_bitset* self_, size_t new_size_, bool fill_);

#endif //PMT_BASE_BITSET_H
