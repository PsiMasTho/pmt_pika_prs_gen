#include "pmt/base/opaque_map.h"

#include "pmt/base/bitset.h"
#include "pmt/base/utility.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pmt_base_opaque_map_internal {
 pmt_base_bitset _in_use;
 size_t (*_fn_hash)(void const*, size_t);
 bool (*_fn_cmp_eq)(void const*, void const*);
 void (*_fn_destroy_key)(void*);
 void (*_fn_destroy_value)(void*);
 void*  _keys;
 void*  _values;
 size_t _size;
 size_t _stride_key;
 size_t _stride_value;
} pmt_base_opaque_map_internal;

typedef union pmt_base_opaque_map_union {
 pmt_base_opaque_map_internal _internal;
 pmt_base_opaque_map          _handle;
} pmt_base_opaque_map_union;

typedef struct pmt_base_opaque_map_find_result {
 size_t _index;
 bool   _found;
} pmt_base_opaque_map_find_result;

static pmt_base_opaque_map_find_result
pmt_base_opaque_map_find(pmt_base_opaque_map const* self_, void const* key_) {
 pmt_base_opaque_map_union self;
 self._handle = *self_;

 size_t index = self._internal._fn_hash(key_, self._internal._stride_key) % pmt_base_opaque_map_capacity(self_);

 while (pmt_base_bitset_at(&self._internal._in_use, index)) {
  if (self._internal._fn_cmp_eq(PMT_BASE_ADVANCE(self._internal._keys, index, self._internal._stride_key), key_))
   return (pmt_base_opaque_map_find_result){index, true};

  index = (index + 1) % pmt_base_opaque_map_capacity(self_);
 }

 return (pmt_base_opaque_map_find_result){index, false};
}

static void
pmt_base_opaque_map_insert_no_grow(pmt_base_opaque_map* self_, void const* key_, void const* value_) {
 pmt_base_opaque_map_union self;
 self._handle = *self_;

 pmt_base_opaque_map_find_result const result = pmt_base_opaque_map_find(self_, key_);
 void*                                 value  = PMT_BASE_ADVANCE(self._internal._values, result._index, self._internal._stride_value);

 if (result._found)
  self._internal._fn_destroy_value(value);
 else {
  memcpy(PMT_BASE_ADVANCE(self._internal._keys, result._index, self._internal._stride_key), key_, self._internal._stride_key);
  pmt_base_bitset_set(&self._internal._in_use, result._index, true);
  ++self._internal._size;
 }

 memcpy(value, value_, self._internal._stride_value);

 *self_ = self._handle;
}

static void
pmt_base_opaque_map_grow(pmt_base_opaque_map* self_) {
 size_t const old_capacity = pmt_base_opaque_map_capacity(self_);
 size_t const new_capacity = pmt_base_grow_capacity(old_capacity, 8, 3, 2);

 pmt_base_opaque_map_union self;
 self._handle = *self_;

 pmt_base_opaque_map_union new_self;
 new_self._handle = pmt_base_opaque_map_create(self._internal._stride_key, self._internal._stride_value, self._internal._fn_destroy_key, self._internal._fn_destroy_value, self._internal._fn_hash, self._internal._fn_cmp_eq);

 new_self._internal._keys   = pmt_base_realloc(NULL, new_capacity * new_self._internal._stride_key);
 new_self._internal._values = pmt_base_realloc(NULL, new_capacity * new_self._internal._stride_value);

 pmt_base_bitset_resize(&new_self._internal._in_use, new_capacity, false);

 // insert the old non-empty elements
 for (size_t i = 0; i < old_capacity; ++i) {
  if (!pmt_base_bitset_at(&self._internal._in_use, i))
   continue;

  pmt_base_opaque_map_insert_no_grow(&new_self._handle, PMT_BASE_ADVANCE(self._internal._keys, i, self._internal._stride_key), PMT_BASE_ADVANCE(self._internal._values, i, self._internal._stride_value));
 }

 pmt_base_swap(&self, &new_self, sizeof(pmt_base_opaque_map_union));
 pmt_base_opaque_map_destroy(&new_self._handle);

 *self_ = self._handle;
}

pmt_base_opaque_map
pmt_base_opaque_map_create(size_t stride_key_, size_t stride_val_, void (*fn_destroy_key_)(void*), void (*fn_destroy_value_)(void*), size_t (*fn_hash_)(void const*, size_t), bool (*fn_cmp_eq_)(void const*, void const*)) {
 pmt_base_opaque_map_union self;

 self._internal._in_use           = pmt_base_bitset_create(0, false);
 self._internal._fn_hash          = fn_hash_;
 self._internal._fn_cmp_eq        = fn_cmp_eq_;
 self._internal._fn_destroy_key   = fn_destroy_key_;
 self._internal._fn_destroy_value = fn_destroy_value_;
 self._internal._keys             = NULL;
 self._internal._values           = NULL;
 self._internal._size             = 0;
 self._internal._stride_key       = stride_key_;
 self._internal._stride_value     = stride_val_;

 return self._handle;
}

void
pmt_base_opaque_map_destroy(void* self_) {
 pmt_base_opaque_map_union self;
 self._handle = *(pmt_base_opaque_map*)self_;

 pmt_base_opaque_map_clear(&self._handle);

 free(self._internal._keys);
 free(self._internal._values);
 pmt_base_bitset_destroy(&self._internal._in_use);
}

void*
pmt_base_opaque_map_at(pmt_base_opaque_map const* self_, void const* key_) {
 pmt_base_opaque_map_find_result const result = pmt_base_opaque_map_find(self_, key_);

 if (!result._found)
  return NULL;

 pmt_base_opaque_map_union self;
 self._handle = *self_;

 return PMT_BASE_ADVANCE(self._internal._values, result._index, self._internal._stride_value);
}

size_t
pmt_base_opaque_map_size(pmt_base_opaque_map const* self_) {
 pmt_base_opaque_map_union self;
 self._handle = *self_;

 return self._internal._size;
}

size_t
pmt_base_opaque_map_capacity(pmt_base_opaque_map const* self_) {
 pmt_base_opaque_map_union self;
 self._handle = *self_;

 return pmt_base_bitset_size(&self._internal._in_use);
}

void
pmt_base_opaque_map_clear(pmt_base_opaque_map* self_) {
 pmt_base_opaque_map_union self;
 self._handle = *self_;

 for (size_t i = 0; i < pmt_base_opaque_map_capacity(self_); ++i) {
  if (!pmt_base_bitset_at(&self._internal._in_use, i))
   continue;

  pmt_base_bitset_set(&self._internal._in_use, i, false);

  self._internal._fn_destroy_key(PMT_BASE_ADVANCE(self._internal._keys, i, self._internal._stride_key));
  self._internal._fn_destroy_value(PMT_BASE_ADVANCE(self._internal._values, i, self._internal._stride_value));
 }

 *self_ = self._handle;
}

void
pmt_base_opaque_map_erase(pmt_base_opaque_map* self_, void const* key_) {
 pmt_base_opaque_map_find_result const tgt_find_result = pmt_base_opaque_map_find(self_, key_);

 if (!tgt_find_result._found)
  return;

 size_t i = tgt_find_result._index;
 size_t j = i;

 pmt_base_opaque_map_union self;
 self._handle = *self_;

 --self._internal._size;
 self._internal._fn_destroy_key(PMT_BASE_ADVANCE(self._internal._keys, i, self._internal._stride_key));
 self._internal._fn_destroy_value(PMT_BASE_ADVANCE(self._internal._values, i, self._internal._stride_value));

 while (1) {
  j = (j + 1) % pmt_base_opaque_map_capacity(self_);

  if (!pmt_base_bitset_at(&self._internal._in_use, j))
   break;

  size_t const k = self._internal._fn_hash(PMT_BASE_ADVANCE(self._internal._keys, j, self._internal._stride_key), self._internal._stride_key) % pmt_base_opaque_map_capacity(self_);

  if ((j > i && (k <= i || k > j)) || (j < i && k <= i && k > j)) {
   memcpy(PMT_BASE_ADVANCE(self._internal._keys, i, self._internal._stride_key), PMT_BASE_ADVANCE(self._internal._keys, j, self._internal._stride_key), self._internal._stride_key);

   memcpy(PMT_BASE_ADVANCE(self._internal._values, i, self._internal._stride_value), PMT_BASE_ADVANCE(self._internal._values, j, self._internal._stride_value), self._internal._stride_value);

   i = j;
  }
 }
 pmt_base_bitset_set(&self._internal._in_use, i, false);
 *self_ = self._handle;
}

void
pmt_base_opaque_map_insert(pmt_base_opaque_map* self_, void const* key_, void const* value_) {
 size_t const threshold = (pmt_base_opaque_map_capacity(self_) / 4) * 3;

 if (pmt_base_opaque_map_size(self_) >= threshold)
  pmt_base_opaque_map_grow(self_);

 pmt_base_opaque_map_insert_no_grow(self_, key_, value_);
}

enum {
 UNINITIALIZED,
 ITERATING,
 AT_END,
};

typedef struct pmt_base_opaque_map_iterator_internal {
 pmt_base_opaque_map const* _container;
 size_t                     _index;
 size_t                     _state;
} pmt_base_opaque_map_iterator_internal;

typedef union pmt_base_opaque_map_iterator_union {
 pmt_base_opaque_map_iterator_internal _internal;
 pmt_base_opaque_map_iterator          _handle;
} pmt_base_opaque_map_iterator_union;

pmt_base_opaque_map_iterator
pmt_base_opaque_map_iterator_create(pmt_base_opaque_map const* container_) {
 pmt_base_opaque_map_iterator_union self;

 self._internal._container = container_;
 self._internal._index     = 0;
 self._internal._state     = UNINITIALIZED;

 pmt_base_opaque_map_iterator_next(&self._handle);

 return self._handle;
}

void const*
pmt_base_opaque_map_iterator_key(pmt_base_opaque_map_iterator const* self_) {
 pmt_base_opaque_map_iterator_union self;
 self._handle = *self_;
 pmt_base_opaque_map_union container;
 container._handle = *self._internal._container;

 if (self._internal._state == AT_END)
  return NULL;

 return PMT_BASE_ADVANCE(container._internal._keys, self._internal._index, container._internal._stride_key);
}

void*
pmt_base_opaque_map_iterator_value(pmt_base_opaque_map_iterator const* self_) {
 pmt_base_opaque_map_iterator_union self;
 self._handle = *self_;
 pmt_base_opaque_map_union container;
 container._handle = *self._internal._container;

 return PMT_BASE_ADVANCE(container._internal._values, self._internal._index, container._internal._stride_value);
}

void
pmt_base_opaque_map_iterator_next(pmt_base_opaque_map_iterator* self_) {
 pmt_base_opaque_map_iterator_union self;
 self._handle = *self_;
 pmt_base_opaque_map_union container;
 container._handle = *self._internal._container;

 switch (self._internal._state) {
 case UNINITIALIZED:
  if (self._internal._index >= pmt_base_opaque_map_capacity(self._internal._container)) {
   self._internal._state = AT_END;
   goto at_exit;
  }
  self._internal._state = ITERATING;
  if (pmt_base_bitset_at(&container._internal._in_use, self._internal._index))
   goto at_exit;
 case ITERATING:
  while (++self._internal._index < pmt_base_opaque_map_capacity(self._internal._container))
   if (pmt_base_bitset_at(&container._internal._in_use, self._internal._index))
    goto at_exit;
  self._internal._state = AT_END;
 case AT_END:
  goto at_exit;
 }

at_exit:
 *self_ = self._handle;
}
