#ifndef PMT_BASE_OPAQUE_VECTOR_H
#define PMT_BASE_OPAQUE_VECTOR_H

#include "pmt/base/opaque_handle.h"

#include <stddef.h>

typedef struct pmt_base_opaque_handle_5 pmt_base_opaque_vector;

pmt_base_opaque_vector
pmt_base_opaque_vector_create(size_t stride_, void (*fn_destroy_)(void*));

pmt_base_opaque_vector
pmt_base_opaque_vector_move(pmt_base_opaque_vector* self_);

void
pmt_base_opaque_vector_destroy(void* self_);

void*
pmt_base_opaque_vector_at(pmt_base_opaque_vector const* self_, size_t pos_);

size_t
pmt_base_opaque_vector_size(pmt_base_opaque_vector const* self_);

size_t
pmt_base_opaque_vector_capacity(pmt_base_opaque_vector const* self_);

void
pmt_base_opaque_vector_clear(pmt_base_opaque_vector* self_);

void
pmt_base_opaque_vector_erase(pmt_base_opaque_vector* self_, size_t pos_, size_t count_);

void
pmt_base_opaque_vector_insert(pmt_base_opaque_vector* self_, size_t pos_, void const* first_, size_t count_);

void
pmt_base_opaque_vector_pop_back(pmt_base_opaque_vector* self_);

void
pmt_base_opaque_vector_push_back(pmt_base_opaque_vector* self_, void const* value_);

void
pmt_base_opaque_vector_reserve(pmt_base_opaque_vector* self_, size_t new_cap_);

void
pmt_base_opaque_vector_shrink_to_fit(pmt_base_opaque_vector* self_);

#endif //PMT_BASE_OPAQUE_VECTOR_H
