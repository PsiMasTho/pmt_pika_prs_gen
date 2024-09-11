#include "pmt/base/utility.h"

#include "pmt/fatal_error.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void
pmt_base_destroy_cstr(void* self_) {
 char** const str = (char**)self_;
 free(*str);
 *str = NULL;
}

void
pmt_base_destroy_trivial(void* self_) {
 (void)self_;
}

bool
pmt_base_eq_cstr(void const* lhs_, void const* rhs_) {
 return strcmp(*(char const* const*)lhs_, *(char const* const*)rhs_) == 0;
}

void*
pmt_base_realloc(void* ptr_, size_t new_size_) {
 void* const p = (ptr_ == NULL ? malloc(new_size_) : realloc(ptr_, new_size_));

 if (p == NULL && new_size_ != 0)
  PMT_FATAL_ERROR_MSG("Memory allocation failed");

 return p;
}

void
pmt_base_free(void* ptr_) {
 free(ptr_);
}

size_t
pmt_base_grow_capacity(size_t current_capacity_, size_t initial_capacity_, size_t rate_num_, size_t rate_den_) {
 // ensure growth
 assert(rate_num_ > rate_den_);

 if (current_capacity_ == 0)
  return initial_capacity_;

 size_t const scaled_capacity = current_capacity_ * rate_num_;

 // Check for overflow
 if (scaled_capacity / current_capacity_ != rate_num_)
  PMT_FATAL_ERROR_MSG("Capacity overflow");

 return scaled_capacity / rate_den_;
}

char*
pmt_base_strdup(char const* str_) {
 size_t const len    = strlen(str_);
 char* const  buffer = pmt_base_realloc(NULL, len + 1);
 memcpy(buffer, str_, len + 1);
 return buffer;
}

char*
pmt_base_strndup(char const* str_, size_t n_) {
 size_t len = strlen(str_);
 len        = PMT_BASE_MIN(len, n_);

 char* const buffer = pmt_base_realloc(NULL, len + 1);
 memcpy(buffer, str_, len);
 buffer[len] = '\0';
 return buffer;
}
