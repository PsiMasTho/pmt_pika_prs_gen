#ifndef PMT_BASE_OPAQUE_MAP_H
#define PMT_BASE_OPAQUE_MAP_H

#include "pmt/base/opaque_handle.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct pmt_base_opaque_handle_11 pmt_base_opaque_map;

pmt_base_opaque_map
pmt_base_opaque_map_create(size_t stride_key_, size_t stride_value_, void (*fn_destroy_key_)(void*), void (*fn_destroy_value_)(void*), size_t (*fn_hash_)(void const*, size_t), bool (*fn_cmp_eq_)(void const*, void const*));

void
pmt_base_opaque_map_destroy(void* self_);

void*
pmt_base_opaque_map_at(pmt_base_opaque_map const* self_, void const* key_);

size_t
pmt_base_opaque_map_size(pmt_base_opaque_map const* self_);

size_t
pmt_base_opaque_map_capacity(pmt_base_opaque_map const* self_);

void
pmt_base_opaque_map_clear(pmt_base_opaque_map* self_);

void
pmt_base_opaque_map_erase(pmt_base_opaque_map* self_, void const* key_);

void
pmt_base_opaque_map_insert(pmt_base_opaque_map* self_, void const* key_, void const* value_);

typedef struct pmt_base_opaque_handle_3 pmt_base_opaque_map_iterator;

pmt_base_opaque_map_iterator
pmt_base_opaque_map_iterator_create(pmt_base_opaque_map const* container_);

/// @note End is reached when returned value is NULL
void const*
pmt_base_opaque_map_iterator_key(pmt_base_opaque_map_iterator const* self_);

void*
pmt_base_opaque_map_iterator_value(pmt_base_opaque_map_iterator const* self_);

void
pmt_base_opaque_map_iterator_next(pmt_base_opaque_map_iterator* self_);

#endif //PMT_BASE_OPAQUE_MAP_H
