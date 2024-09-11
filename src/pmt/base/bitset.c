#include "pmt/base/bitset.h"

#include "utility.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint_fast8_t data_type;

enum {
 DATA_TYPE_BITS = sizeof(data_type) * CHAR_BIT
};

typedef struct pmt_base_bitset_internal {
 data_type* _data;
 size_t     _size; // in bits
} pmt_base_bitset_internal;

typedef union pmt_base_bitset_union {
 pmt_base_bitset_internal _internal;
 pmt_base_bitset          _handle;
} pmt_base_bitset_union;

typedef struct location {
 size_t  _data_index;
 uint8_t _bit_index;
} location;

static size_t
get_required_alloc_size(size_t index_) {
 return (index_ + DATA_TYPE_BITS - 1) / DATA_TYPE_BITS;
}

static size_t
get_data_index(size_t index_) {
 return index_ / DATA_TYPE_BITS;
}

static size_t
get_bit_index(size_t index_) {
 return index_ % DATA_TYPE_BITS;
}

static void
masked_set(data_type* dest_, data_type mask_, bool value_) {
 if (value_)
  *dest_ |= mask_;
 else
  *dest_ &= ~mask_;
}

pmt_base_bitset
pmt_base_bitset_create(size_t size_, bool fill_) {
 pmt_base_bitset_union self;
 self._internal._data = NULL;
 self._internal._size = 0;

 if (size_ != 0)
  pmt_base_bitset_resize(&self._handle, size_, fill_);

 return self._handle;
}

void
pmt_base_bitset_destroy(void* self_) {
 pmt_base_bitset_union self;
 self._handle = *(pmt_base_bitset*)self_;
 free(self._internal._data);
}

bool
pmt_base_bitset_at(pmt_base_bitset const* self_, size_t pos_) {
 pmt_base_bitset_union self;
 self._handle = *self_;
 return (self._internal._data[get_data_index(pos_)] & (1ull << (DATA_TYPE_BITS - get_bit_index(pos_) - 1)));
}

size_t
pmt_base_bitset_size(pmt_base_bitset const* self_) {
 pmt_base_bitset_union self;
 self._handle = *self_;
 return self._internal._size;
}

void
pmt_base_bitset_set(pmt_base_bitset* self_, size_t pos_, bool value_) {
 pmt_base_bitset_union self;
 self._handle = *self_;
 masked_set(self._internal._data + get_data_index(pos_), 1ull << (DATA_TYPE_BITS - get_bit_index(pos_) - 1), value_);
 *self_ = self._handle;
}

void
pmt_base_bitset_resize(pmt_base_bitset* self_, size_t new_size_, bool fill_) {
 pmt_base_bitset_union self;
 self._handle = *self_;

 size_t old_size = self._internal._size;
 if (new_size_ == old_size)
  goto at_exit;

 self._internal._size = new_size_;

 size_t const new_alloc_size = get_required_alloc_size(new_size_);
 size_t const old_alloc_size = self._internal._data == NULL ? 0 : get_required_alloc_size(old_size);

 if (new_alloc_size != old_alloc_size)
  self._internal._data = pmt_base_realloc(self._internal._data, new_alloc_size);

 if (new_size_ < old_size)
  goto at_exit;

 while (old_size < new_size_) {
  size_t const bit_start = get_bit_index(old_size);
  size_t const bit_diff  = PMT_BASE_MIN(new_size_ - old_size, DATA_TYPE_BITS - bit_start);

  data_type mask = ~0;
  mask <<= DATA_TYPE_BITS - bit_diff;
  mask >>= bit_start;
  masked_set(self._internal._data + get_data_index(old_size), mask, fill_);
  old_size += bit_diff;
 }

at_exit:
 *self_ = self._handle;
}
