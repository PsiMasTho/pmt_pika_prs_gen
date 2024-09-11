#include <pmt/base/opaque_vector.h>
#include <pmt/base/utility.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct ov_test_struct
{
  int   _num;
  char* _str;
} ov_test_struct; // opaque vector test struct

static char const s_sample_str_1[] = "this is a sample string";
static char const s_sample_str_2[] = "this is another sample string";

static void ov_test_struct_destroy(void* self_)
{
  assert(self_ != NULL);
  ov_test_struct* const s = (ov_test_struct*)self_;
  free(s->_str);
  s->_str = NULL;
}

static void pmt_base_opaque_vector_test_2d(void)
{
  pmt_base_opaque_vector v
    = pmt_base_opaque_vector_create(sizeof(pmt_base_opaque_vector), pmt_base_opaque_vector_destroy);

  for (int i = 0; i < 100; ++i)
  {
    pmt_base_opaque_vector v2 = pmt_base_opaque_vector_create(sizeof(int), pmt_base_destroy_trivial);
    for (int j = 0; j < 10; ++j)
      pmt_base_opaque_vector_push_back(&v2, &j);
    pmt_base_opaque_vector_push_back(&v, &v2);
  }

  for (int i = 0; i < 100; ++i)
  {
    pmt_base_opaque_vector* const v2 = pmt_base_opaque_vector_at(&v, i);
    for (int j = 0; j < 10; ++j)
    {
      int* const n = pmt_base_opaque_vector_at(v2, j);
      assert(*n == j);
    }
  }

  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_c_str(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(char*), pmt_base_destroy_cstr);
  char*                  s = strdup("hello world");
  pmt_base_opaque_vector_push_back(&v, &s);
  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_clear(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(ov_test_struct), ov_test_struct_destroy);
  for (int i = 0; i < 100; ++i)
  {
    char* sample_str = malloc(sizeof(s_sample_str_1));
    strncpy(sample_str, s_sample_str_1, sizeof(s_sample_str_1));
    ov_test_struct s = { ._num = i, ._str = sample_str };
    pmt_base_opaque_vector_push_back(&v, &s);
  }
  pmt_base_opaque_vector_clear(&v);

  assert(pmt_base_opaque_vector_size(&v) == 0);
  assert(pmt_base_opaque_vector_capacity(&v) > 0);

  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_create(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(ov_test_struct), ov_test_struct_destroy);
  assert(pmt_base_opaque_vector_size(&v) == 0);
  assert(pmt_base_opaque_vector_capacity(&v) == 0);
  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_erase_many(void)
{
  int arr_initial[]  = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  int arr_expected[] = { 1, 2, 3, 8, 9, 10 };

  pmt_base_opaque_vector ov_initial = pmt_base_opaque_vector_create(sizeof(int), pmt_base_destroy_trivial);
  for (size_t i = 0; i < sizeof(arr_initial) / sizeof(arr_initial[0]); ++i)
    pmt_base_opaque_vector_push_back(&ov_initial, arr_initial + i);

  // erase indices [3, 7]
  pmt_base_opaque_vector_erase(&ov_initial, 3, 4);

  assert(pmt_base_opaque_vector_size(&ov_initial) == sizeof(arr_expected) / sizeof(arr_expected[0]));

  for (size_t i = 0; i < pmt_base_opaque_vector_size(&ov_initial); ++i)
    assert(*(int*)pmt_base_opaque_vector_at(&ov_initial, i) == arr_expected[i]);

  pmt_base_opaque_vector_destroy(&ov_initial);
}

static void pmt_base_opaque_vector_test_erase_one(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(int), pmt_base_destroy_trivial);
  for (size_t i = 0; i < 100; ++i)
    pmt_base_opaque_vector_push_back(&v, &i);

  for (size_t i = 0; i < pmt_base_opaque_vector_size(&v);)
    if (*(int*)pmt_base_opaque_vector_at(&v, i) % 2 == 0)
      pmt_base_opaque_vector_erase(&v, i, 1);
    else
      ++i;

  assert(pmt_base_opaque_vector_size(&v) == 50);
  assert(pmt_base_opaque_vector_capacity(&v) >= 50);

  for (int i = 0; i < 50; ++i)
  {
    int* const n = pmt_base_opaque_vector_at(&v, i);
    assert(*n == i * 2 + 1);
  }

  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_insert_many(void)
{
  pmt_base_opaque_vector vi = pmt_base_opaque_vector_create(sizeof(int), pmt_base_destroy_trivial);
  for (int i = 0; i < 100; ++i)
    pmt_base_opaque_vector_push_back(&vi, &i);

  int const    to_insert[]    = { 666, 777, 888, 999 };
  size_t const to_insert_size = sizeof(to_insert) / sizeof(to_insert[0]);

  pmt_base_opaque_vector_insert(&vi, 10, to_insert, to_insert_size);

  assert(pmt_base_opaque_vector_size(&vi) == 100 + to_insert_size);

  for (int i = 0; i < 10; ++i)
  {
    int* const n = pmt_base_opaque_vector_at(&vi, i);
    assert(*n == i);
  }

  for (size_t i = 0; i < to_insert_size; ++i)
  {
    int* const n = pmt_base_opaque_vector_at(&vi, 10 + i);
    assert(*n == to_insert[i]);
  }

  for (int i = 0; i < 90; ++i)
  {
    int* const n = pmt_base_opaque_vector_at(&vi, 10 + to_insert_size + i);
    assert(*n == i + 10);
  }

  pmt_base_opaque_vector_destroy(&vi);
}

static void pmt_base_opaque_vector_test_insert_one(void)
{
  pmt_base_opaque_vector vi = pmt_base_opaque_vector_create(sizeof(int), pmt_base_destroy_trivial);
  int                    n  = 42;
  pmt_base_opaque_vector_insert(&vi, 0, &n, 1);

  assert(pmt_base_opaque_vector_size(&vi) == 1);
  assert(pmt_base_opaque_vector_capacity(&vi) > 0);

  int* const n2 = pmt_base_opaque_vector_at(&vi, 0);
  assert(*n2 == 42);

  pmt_base_opaque_vector_destroy(&vi);
}

static void pmt_base_opaque_vector_test_pop(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(ov_test_struct), ov_test_struct_destroy);
  ov_test_struct         s = { ._num = 42, ._str = NULL };
  pmt_base_opaque_vector_push_back(&v, &s);
  pmt_base_opaque_vector_pop_back(&v);

  assert(pmt_base_opaque_vector_size(&v) == 0);
  assert(pmt_base_opaque_vector_capacity(&v) > 0);

  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_push_back_many(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(ov_test_struct), ov_test_struct_destroy);
  for (int i = 0; i < 100; ++i)
  {
    char* sample_str = malloc(sizeof(s_sample_str_1));
    strncpy(sample_str, s_sample_str_1, sizeof(s_sample_str_1));
    ov_test_struct s = { ._num = i, ._str = sample_str };
    pmt_base_opaque_vector_push_back(&v, &s);
  }

  assert(pmt_base_opaque_vector_size(&v) == 100);
  assert(pmt_base_opaque_vector_capacity(&v) >= 100);

  for (int i = 0; i < 100; ++i)
  {
    ov_test_struct* const s = pmt_base_opaque_vector_at(&v, i);
    assert(s->_num == i);
    assert(strcmp(s->_str, s_sample_str_1) == 0);
  }
  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_push_back_one(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(ov_test_struct), ov_test_struct_destroy);
  ov_test_struct         s = { ._num = 42, ._str = NULL };
  pmt_base_opaque_vector_push_back(&v, &s);

  assert(pmt_base_opaque_vector_size(&v) == 1);
  assert(pmt_base_opaque_vector_capacity(&v) > 0);

  ov_test_struct* const s2 = pmt_base_opaque_vector_at(&v, 0);
  assert(s2->_num == 42);
  assert(s2->_str == NULL);
  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_push_back_pop_alternate(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(ov_test_struct), ov_test_struct_destroy);
  for (int i = 0; i < 100; ++i)
  {
    char* sample_str = malloc(sizeof(s_sample_str_1));
    strncpy(sample_str, s_sample_str_1, sizeof(s_sample_str_1));
    ov_test_struct s = { ._num = i, ._str = sample_str };
    pmt_base_opaque_vector_push_back(&v, &s);
  }

  for (int i = 0; i < 100; ++i)
  {
    ov_test_struct* const s = pmt_base_opaque_vector_at(&v, i);
    assert(s->_num == i);
    assert(strcmp(s->_str, s_sample_str_1) == 0);
  }

  for (int i = 0; i < 100; ++i)
    pmt_base_opaque_vector_pop_back(&v);

  for (int i = 0; i < 100; ++i)
  {
    char* sample_str = malloc(sizeof(s_sample_str_2));
    strncpy(sample_str, s_sample_str_2, sizeof(s_sample_str_2));
    ov_test_struct s = { ._num = i, ._str = sample_str };
    pmt_base_opaque_vector_push_back(&v, &s);
  }

  for (int i = 0; i < 100; ++i)
  {
    ov_test_struct* const s = pmt_base_opaque_vector_at(&v, i);
    assert(s->_num == i);
    assert(strcmp(s->_str, s_sample_str_2) == 0);
  }

  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_reserve(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(ov_test_struct), ov_test_struct_destroy);
  pmt_base_opaque_vector_reserve(&v, 100);

  assert(pmt_base_opaque_vector_size(&v) == 0);
  assert(pmt_base_opaque_vector_capacity(&v) == 100);

  pmt_base_opaque_vector_destroy(&v);
}

static void pmt_base_opaque_vector_test_shrink_to_fit(void)
{
  pmt_base_opaque_vector v = pmt_base_opaque_vector_create(sizeof(float), pmt_base_destroy_trivial);

  for (int i = 0; i < 100; ++i)
  {
    float const f = i + 3.14159f;
    pmt_base_opaque_vector_push_back(&v, &f);
  }

  assert(pmt_base_opaque_vector_size(&v) == 100);
  assert(pmt_base_opaque_vector_capacity(&v) >= 100);

  pmt_base_opaque_vector_shrink_to_fit(&v);

  assert(pmt_base_opaque_vector_size(&v) == 100);
  assert(pmt_base_opaque_vector_capacity(&v) == 100);

  pmt_base_opaque_vector_clear(&v);
  pmt_base_opaque_vector_shrink_to_fit(&v);

  assert(pmt_base_opaque_vector_size(&v) == 0);
  assert(pmt_base_opaque_vector_capacity(&v) == 0);

  pmt_base_opaque_vector_push_back(&v, &(float){ 3.14159f });

  assert(pmt_base_opaque_vector_size(&v) == 1);
  assert(pmt_base_opaque_vector_capacity(&v) >= 1);

  pmt_base_opaque_vector_destroy(&v);
}

void pmt_base_opaque_vector_test(void)
{
  pmt_base_opaque_vector_test_2d();
  pmt_base_opaque_vector_test_c_str();
  pmt_base_opaque_vector_test_clear();
  pmt_base_opaque_vector_test_create();
  pmt_base_opaque_vector_test_erase_many();
  pmt_base_opaque_vector_test_erase_one();
  pmt_base_opaque_vector_test_insert_many();
  pmt_base_opaque_vector_test_insert_one();
  pmt_base_opaque_vector_test_pop();
  pmt_base_opaque_vector_test_push_back_many();
  pmt_base_opaque_vector_test_push_back_one();
  pmt_base_opaque_vector_test_push_back_pop_alternate();
  pmt_base_opaque_vector_test_reserve();
  pmt_base_opaque_vector_test_shrink_to_fit();
}
