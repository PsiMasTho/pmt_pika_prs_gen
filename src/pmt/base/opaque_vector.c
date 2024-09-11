#include "pmt/base/opaque_vector.h"

#include "pmt/base/utility.h"
#include "pmt/fatal_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pmt_base_opaque_vector_internal {
 void (*_fn_destroy)(void*);
 void*  _begin;
 void*  _end;
 void*  _end_cap;
 size_t _stride;
} pmt_base_opaque_vector_internal;

typedef union pmt_base_opaque_vector_union {
 pmt_base_opaque_vector_internal _internal;
 pmt_base_opaque_vector          _handle;
} pmt_base_opaque_vector_union;

pmt_base_opaque_vector
pmt_base_opaque_vector_create(size_t stride_, void (*fn_destroy_)(void*)) {
 pmt_base_opaque_vector_union self;
 self._internal._fn_destroy = fn_destroy_;
 self._internal._begin = self._internal._end = self._internal._end_cap = NULL;
 self._internal._stride                                                = stride_;
 return self._handle;
}

void
pmt_base_opaque_vector_destroy(void* self_) {
 pmt_base_opaque_vector_union self;
 self._handle = *(pmt_base_opaque_vector*)self_;

 pmt_base_opaque_vector_clear(&self._handle);

 free(self._internal._begin);
}

void*
pmt_base_opaque_vector_at(pmt_base_opaque_vector const* self_, size_t pos_) {
 pmt_base_opaque_vector_union self;
 self._handle = *self_;
 return PMT_BASE_ADVANCE(self._internal._begin, pos_, self._internal._stride);
}

size_t
pmt_base_opaque_vector_size(pmt_base_opaque_vector const* self_) {
 pmt_base_opaque_vector_union self;
 self._handle = *self_;
 return pmt_base_distance(self._internal._begin, self._internal._end, self._internal._stride);
}

size_t
pmt_base_opaque_vector_capacity(pmt_base_opaque_vector const* self_) {
 pmt_base_opaque_vector_union self;
 self._handle = *self_;
 return pmt_base_distance(self._internal._begin, self._internal._end_cap, self._internal._stride);
}

void
pmt_base_opaque_vector_clear(pmt_base_opaque_vector* self_) {
 while (pmt_base_opaque_vector_size(self_) != 0)
  pmt_base_opaque_vector_pop_back(self_);
}

void
pmt_base_opaque_vector_erase(pmt_base_opaque_vector* self_, size_t pos_, size_t count_) {
 pmt_base_opaque_vector_union self;
 self._handle = *self_;

 for (size_t i = 0; i < count_; ++i)
  self._internal._fn_destroy(pmt_base_opaque_vector_at(self_, pos_ + i));

 memmove(PMT_BASE_ADVANCE(self._internal._begin, pos_, self._internal._stride), PMT_BASE_ADVANCE(self._internal._begin, pos_ + count_, self._internal._stride), self._internal._stride * (pmt_base_opaque_vector_size(self_) - pos_ - count_));

 self._internal._end = PMT_BASE_ADVANCE(self._internal._end, -count_, self._internal._stride);
 *self_              = self._handle;
}

void
pmt_base_opaque_vector_insert(pmt_base_opaque_vector* self_, size_t pos_, void const* first_, size_t count_) {
 // reserve space if needed
 if (pmt_base_opaque_vector_capacity(self_) < pmt_base_opaque_vector_size(self_) + count_) {
  size_t needed_capacity = pmt_base_opaque_vector_capacity(self_);
  do {
   needed_capacity = pmt_base_grow_capacity(needed_capacity, 4, 3, 2);
  } while (needed_capacity < pmt_base_opaque_vector_size(self_) + count_);

  pmt_base_opaque_vector_reserve(self_, needed_capacity);
 }

 pmt_base_opaque_vector_union self;
 self._handle = *self_;

 // move elements to make space
 memmove(PMT_BASE_ADVANCE(self._internal._begin, pos_ + count_, self._internal._stride), PMT_BASE_ADVANCE(self._internal._begin, pos_, self._internal._stride), self._internal._stride * (pmt_base_opaque_vector_size(self_) - pos_));

 // copy elements into the new space
 memcpy(PMT_BASE_ADVANCE(self._internal._begin, pos_, self._internal._stride), first_, self._internal._stride * count_);

 self._internal._end = PMT_BASE_ADVANCE(self._internal._end, count_, self._internal._stride);
 *self_              = self._handle;
}

void
pmt_base_opaque_vector_pop_back(pmt_base_opaque_vector* self_) {
 pmt_base_opaque_vector_erase(self_, pmt_base_opaque_vector_size(self_) - 1, 1);
}

void
pmt_base_opaque_vector_push_back(pmt_base_opaque_vector* self_, void const* value_) {
 pmt_base_opaque_vector_insert(self_, pmt_base_opaque_vector_size(self_), value_, 1);
}

void
pmt_base_opaque_vector_reserve(pmt_base_opaque_vector* self_, size_t new_cap_) {
 if (new_cap_ <= pmt_base_opaque_vector_capacity(self_))
  return;

 size_t const size = pmt_base_opaque_vector_size(self_);

 pmt_base_opaque_vector_union self;
 self._handle = *self_;

 self._internal._begin   = pmt_base_realloc(self._internal._begin, new_cap_ * self._internal._stride);
 self._internal._end     = PMT_BASE_ADVANCE(self._internal._begin, size, self._internal._stride);
 self._internal._end_cap = PMT_BASE_ADVANCE(self._internal._begin, new_cap_, self._internal._stride);

 *self_ = self._handle;
}

void
pmt_base_opaque_vector_shrink_to_fit(pmt_base_opaque_vector* self_) {
 size_t const size = pmt_base_opaque_vector_size(self_);

 pmt_base_opaque_vector_union self;
 self._handle = *self_;

 self._internal._begin = pmt_base_realloc(self._internal._begin, size * self._internal._stride);
 self._internal._end = self._internal._end_cap = PMT_BASE_ADVANCE(self._internal._begin, size, self._internal._stride);

 *self_ = self._handle;
}
