#ifndef PMT_BASE_UTILITY_H
#define PMT_BASE_UTILITY_H

#include <stdbool.h>
#include <stddef.h>

#define PMT_BASE_UCLIP(X_, MAX_) (((X_) > (MAX_)) ? (MAX_) : (X_))
#define PMT_BASE_LCLIP(X_, MIN_) (((X_) < (MIN_)) ? (MIN_) : (X_))
#define PMT_BASE_MIN(A_, B_) (((A_) < (B_)) ? (A_) : (B_))
#define PMT_BASE_MAX(A_, B_) (((A_) > (B_)) ? (A_) : (B_))
#define PMT_BASE_ADVANCE(P_, N_, STRIDE_) ((char*)(P_) + (N_) * (STRIDE_))

static inline bool
pmt_base_between(void const* begin_, void const* end_, void const* target_) {
 return begin_ <= target_ && target_ < end_;
}

static inline size_t
pmt_base_distance(void const* first_, void const* last_, size_t stride_) {
 char const* const first = first_;
 char const* const last  = last_;
 return (last - first) / stride_;
}

static inline void
pmt_base_swap(void* lhs_, void* rhs_, size_t stride_) {
 char* lhs = (char*)(lhs_);
 char* rhs = (char*)(rhs_);
 while (stride_--) {
  char const tmp = *lhs;
  *lhs++         = *rhs;
  *rhs++         = tmp;
 }
}

///@brief Destroy a pointer to a C string. i.e. char**
void
pmt_base_destroy_cstr(void* self_);

///@brief Destroy a trivial pointer. (Does nothing)
void
pmt_base_destroy_trivial(void* self_);

bool
pmt_base_eq_cstr(void const* lhs_, void const* rhs_);

///@brief (Re) Allocate memory.
///
///@param ptr_      Pointer to the memory to reallocate. If NULL, allocate new memory.
///                 Otherwise, reallocate the memory.
///@param new_size_ New size of the memory.
void*
pmt_base_alloc(void* ptr_, size_t new_size_);

void
pmt_base_free(void* ptr_);

///@brief Grow the capacity of a container.
///
///@param current_capacity_ Current capacity of the container.
///@param initial_capacity_ Capacity to return if the current capacity is zero.
///@param rate_num_         Numerator of the growth rate.
///@param rate_den_         Denominator of the growth rate.
///@return                  New capacity.
///
///@example pmt_grow_capacity(0, 33, 3, 2)  returns 33
///@example pmt_grow_capacity(100, 1, 3, 2) returns 150
size_t
pmt_base_grow_capacity(size_t current_capacity_, size_t initial_capacity_, size_t rate_num_, size_t rate_den_);

char*
pmt_base_strdup(char const* str_);

char*
pmt_base_strndup(char const* str_, size_t n_);

#endif //PMT_BASE_UTILITY_H
