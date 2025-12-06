#pragma once

namespace pmt::base::test {

class MultiVectorTest {
public:
 static void run();

private:
 static void test_push_pop_and_access();
 static void test_insert_erase_and_swap();
 static void test_resize_and_clear();
 static void test_copy_and_equality();
 static void test_heap_alloc_types();
 static void test_get_tuple();
};

};  // namespace pmt::base::test
