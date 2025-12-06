#include "pmt/base/test/multi_vector_test.hpp"

#include "pmt/base/multi_vector.hpp"

#include <cassert>
#include <cstddef>
#include <string>
#include <vector>

namespace pmt::base::test {

void MultiVectorTest::run() {
 test_push_pop_and_access();
 test_insert_erase_and_swap();
 test_resize_and_clear();
 test_copy_and_equality();
 test_heap_alloc_types();
 test_get_tuple();
}

void MultiVectorTest::test_push_pop_and_access() {
 MultiVector<int, std::string> multi_vector;
 assert(multi_vector.empty());
 assert(multi_vector.size() == 0);

 multi_vector.push_back(10, "ten");
 multi_vector.push_back(20, "twenty");
 multi_vector.push_back(30, "thirty");

 assert(multi_vector.size() == 3);
 assert(multi_vector.get_span_by_index<0>()[0] == 10);
 assert(multi_vector.get_span_by_index<0>()[2] == 30);
 assert(multi_vector.get_span_by_type<std::string>()[1] == "twenty");

 multi_vector.swap_elements(0, 2);
 assert(multi_vector.get_span_by_index<0>()[0] == 30);
 assert(multi_vector.get_span_by_index<1>()[0] == "thirty");
 assert(multi_vector.get_span_by_index<0>()[2] == 10);

 multi_vector.pop_back();
 assert(multi_vector.size() == 2);
 assert(multi_vector.get_span_by_type<int>()[0] == 30);
 assert(multi_vector.get_span_by_type<std::string>()[0] == "thirty");

 multi_vector.pop_back();
 multi_vector.pop_back();
 assert(multi_vector.empty());
}

void MultiVectorTest::test_insert_erase_and_swap() {
 MultiVector<int, std::string> multi_vector;
 multi_vector.push_back(1, "one");
 multi_vector.push_back(3, "three");
 multi_vector.insert(1, 2, "two");

 assert(multi_vector.size() == 3);
 assert(multi_vector.get_span_by_index<0>()[1] == 2);
 assert(multi_vector.get_span_by_type<std::string>()[1] == "two");

 multi_vector.erase(0);
 assert(multi_vector.size() == 2);
 assert(multi_vector.get_span_by_index<0>()[0] == 2);

 multi_vector.swap_elements(0, 1);
 assert(multi_vector.get_span_by_index<0>()[0] == 3);
 assert(multi_vector.get_span_by_index<1>()[0] == "three");
}

void MultiVectorTest::test_resize_and_clear() {
 MultiVector<int, std::string> multi_vector;
 multi_vector.push_back(1, "one");
 multi_vector.push_back(2, "two");

 multi_vector.resize(5, 42, "fill");
 assert(multi_vector.size() == 5);
 for (size_t i = 2; i < multi_vector.size(); ++i) {
  assert(multi_vector.get_span_by_index<0>()[i] == 42);
  assert(multi_vector.get_span_by_type<std::string>()[i] == "fill");
 }

 multi_vector.resize(1, 0, "");
 assert(multi_vector.size() == 1);
 assert(multi_vector.get_span_by_index<0>()[0] == 1);

 multi_vector.clear();
 assert(multi_vector.empty());
 assert(multi_vector.size() == 0);
}

void MultiVectorTest::test_copy_and_equality() {
 MultiVector<int, std::string> original;
 original.push_back(10, "ten");
 original.push_back(20, "twenty");
 original.push_back(30, "thirty");

 MultiVector<int, std::string> copy(original);
 assert(copy == original);

 copy.get_span_by_index<0>()[1] = 200;
 copy.get_span_by_index<1>()[1] = "mutated";

 assert(copy != original);
 assert(original.get_span_by_index<0>()[1] == 20);
 assert(original.get_span_by_type<std::string>()[1] == "twenty");

 MultiVector<int, std::string> assigned;
 assigned = copy;
 assert(assigned == copy);

 copy.pop_back();
 assert(copy.size() == 2);
 assert(assigned.size() == 3);
 assert(copy != assigned);
}

void MultiVectorTest::test_heap_alloc_types() {
 MultiVector<std::vector<int>, std::string> multi_vector;
 assert(multi_vector.empty());

 multi_vector.push_back(std::vector<int>{1, 2}, "first");
 multi_vector.push_back(std::vector<int>{3, 4, 5}, "second");
 assert(multi_vector.size() == 2);
 assert(multi_vector.get_span_by_index<0>()[0].size() == 2);
 assert(multi_vector.get_span_by_index<1>()[1] == "second");

 multi_vector.get_span_by_index<0>()[1].push_back(6);
 assert(multi_vector.get_span_by_index<0>()[1].size() == 4);

 multi_vector.insert(1, std::vector<int>{7, 8}, "inserted");
 assert(multi_vector.size() == 3);
 assert(multi_vector.get_span_by_index<0>()[1].front() == 7);
 assert(multi_vector.get_span_by_type<std::string>()[1] == "inserted");

 multi_vector.erase(0);
 assert(multi_vector.size() == 2);
 assert(multi_vector.get_span_by_type<std::string>()[0] == "inserted");

 multi_vector.resize(4, std::vector<int>{9}, "fill");
 assert(multi_vector.size() == 4);
 for (size_t i = 2; i < multi_vector.size(); ++i) {
  assert(multi_vector.get_span_by_index<0>()[i].size() == 1);
  assert(multi_vector.get_span_by_index<0>()[i].front() == 9);
  assert(multi_vector.get_span_by_index<1>()[i] == "fill");
 }

 multi_vector.clear();
 assert(multi_vector.empty());
 assert(multi_vector.size() == 0);
}

void MultiVectorTest::test_get_tuple() {
 MultiVector<int, std::string> multi_vector;
 multi_vector.push_back(42, "answer");

 auto [num, str] = multi_vector.get_tuple(0);
 assert(num == 42);
 assert(str == "answer");

 num = 100;
 str = "changed";

 auto [num2, str2] = multi_vector.get_tuple(0);
 assert(num2 == 100);
 assert(str2 == "changed");
}

}  // namespace pmt::base::test
