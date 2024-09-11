#include <pmt/base/opaque_map.h>

#include <pmt/base/hash.h>
#include <pmt/base/utility.h>

#include <assert.h>
#include <stdio.h>

typedef int    test_key_type;
typedef size_t test_value_type;

static void map_insert(pmt_base_opaque_map* m_, test_key_type k_, test_value_type v_)
{
  pmt_base_opaque_map_insert(m_, &k_, &v_);
}

static test_value_type map_get(pmt_base_opaque_map* m_, test_key_type k_)
{
  test_value_type const* result = pmt_base_opaque_map_at(m_, &k_);

  assert(result != NULL);

  return *result;
}

static bool map_contains(pmt_base_opaque_map* m_, test_key_type k_)
{
  return pmt_base_opaque_map_at(m_, &k_) != NULL;
}

static void map_erase(pmt_base_opaque_map* m_, test_key_type k_)
{
  pmt_base_opaque_map_erase(m_, &k_);
}

static void map_print(pmt_base_opaque_map* m_)
{
  pmt_base_opaque_map_iterator it = pmt_base_opaque_map_iterator_create(m_);

  printf("----------");
  while (1)
  {
    test_key_type const* k = pmt_base_opaque_map_iterator_key(&it);

    if (!k)
      break;

    test_value_type const* v = pmt_base_opaque_map_iterator_value(&it);

    printf("[%d : %zu]\n", *k, *v);
    pmt_base_opaque_map_iterator_next(&it);
  }

  printf("SIZE: %zu\nCAPACITY: %zu\n----------\n", pmt_base_opaque_map_size(m_), pmt_base_opaque_map_capacity(m_));
}

bool eq_test_key_type(void const* a_, void const* b_)
{
  return *(test_key_type const*)a_ == *(test_key_type const*)b_;
}

size_t bad_hash(void const* data_, size_t size_)
{
  unsigned char const* data = data_;
  return *data;
}

void pmt_base_opaque_map_test(void)
{
  pmt_base_opaque_map map = pmt_base_opaque_map_create(
    sizeof(test_key_type),
    sizeof(test_value_type),
    pmt_base_destroy_trivial,
    pmt_base_destroy_trivial,
    pmt_base_hash_fnv1a,
    eq_test_key_type);

  size_t const count = 1000000;
  for (size_t i = 0; i < count; ++i)
    map_insert(&map, i, i * 10);

  // map_print(map);

  for (size_t i = 0; i < count; ++i)
  {
    assert(map_contains(&map, i));
    assert(map_get(&map, i) == i * 10);
  }

  assert(pmt_base_opaque_map_size(&map) == count);

  size_t expected_size = count;
  for (size_t i = 0; i < count; ++i)
  {
    if (i % 2 == 0)
    {
      // printf("SIZE BEFORE ERASE: %zu\n", pmt_base_opaque_map_size(map));
      // assert(map_contains(map, i));
      map_erase(&map, i);
      // assert(!map_contains(map, i));

      --expected_size;
    }
  }

  // map_print(map);

  // printf("EXPECTED SIZE: %zu\n", expected_size);

  assert(pmt_base_opaque_map_size(&map) == expected_size);

  pmt_base_opaque_map_destroy(&map);
}
