#include <pmt/base/bitset.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
// #include <time.h>

static void append(pmt_base_bitset* bitset_, bool value_)
{
  pmt_base_bitset_resize(bitset_, pmt_base_bitset_size(bitset_) + 1, value_);
}

static void print_bitset(pmt_base_bitset* bitset_)
{
  for (size_t i = 0; i < pmt_base_bitset_size(bitset_); ++i)
    printf("%d", pmt_base_bitset_at(bitset_, i));
  printf("\n");
}

void pmt_base_bitset_test(void)
{
  // srand(time(NULL));
  size_t const count = rand() % 64000;
  size_t const mod   = rand() % 64;

  pmt_base_bitset bitset = pmt_base_bitset_create(0, false);

  for (size_t i = 0; i < count; ++i)
    append(&bitset, i % mod == 0);

  for (size_t i = 0; i < count; ++i)
    assert(pmt_base_bitset_at(&bitset, i) == i % mod);

  for (size_t i = 0; i < count; ++i)
    pmt_base_bitset_set(&bitset, i, false);

  for (size_t i = 0; i < count; ++i)
    assert(pmt_base_bitset_at(&bitset, i) == false);

  for (size_t i = 0; i < count; ++i)
    pmt_base_bitset_set(&bitset, i, true);

  for (size_t i = 0; i < count; ++i)
    assert(pmt_base_bitset_at(&bitset, i) == true);

  for (size_t i = 0; i < count; ++i)
    pmt_base_bitset_set(&bitset, i, false);

  for (size_t i = 0; i < count; ++i)
    assert(pmt_base_bitset_at(&bitset, i) == false);

  pmt_base_bitset_resize(&bitset, 0, false);
  pmt_base_bitset_resize(&bitset, 1111, false);

  assert(pmt_base_bitset_at(&bitset, 256) == false);
  pmt_base_bitset_set(&bitset, 256, true);
  assert(pmt_base_bitset_at(&bitset, 256) == true);

  pmt_base_bitset_destroy(&bitset);
}
