#include "pmt/base/test/dynamic_bitset_test.hpp"

#include "pmt/base/dynamic_bitset.hpp"
#include "pmt/base/dynamic_bitset_converter.hpp"

#include <cassert>

namespace pmt::base::test {

void DynamicBitsetTest::run() {
  test_lifetime_functions();
  test_capacity_functions();
  test_single_bit_access_functions();
  test_equality();
  test_bitwise_operations();
}

void DynamicBitsetTest::test_lifetime_functions() {
  DynamicBitset b1 = DynamicBitset(10, true);
  DynamicBitset b2 = DynamicBitset(b1);
  DynamicBitset b3 = DynamicBitset(20, false);

  assert(b1.size() == 10);
  assert(b2.size() == 10);
  assert(b3.size() == 20);

  DynamicBitset b4 = std::move(b1);
  assert(b4.size() == 10);
  assert(b1.size() == 0);

  b1 = b2;
  assert(b1.size() == 10);
  assert(b2.size() == 10);

  b1 = std::move(b2);
  assert(b1.size() == 10);
  assert(b2.size() == 0);
}

void DynamicBitsetTest::test_capacity_functions() {
  DynamicBitset b1 = DynamicBitset(10, true);
  assert(b1.size() == 10);
  assert(b1.capacity() >= 10);

  b1.reserve(20);
  assert(b1.size() == 10);
  assert(b1.capacity() >= 20);

  b1.resize(5);
  assert(b1.size() == 5);
  assert(b1.capacity() >= 20);

  b1.clear();
  assert(b1.size() == 0);
  assert(b1.capacity() >= 20);

  b1.resize(10, true);
  assert(b1.size() == 10);
  assert(b1.capacity() >= 20);

  b1.push_back(true);
  assert(b1.size() == 11);
  assert(b1.capacity() >= 20);

  b1.pop_back();
  assert(b1.size() == 10);
  assert(b1.capacity() >= 20);

  for (size_t i = 0; i < 1024; ++i) {
    b1.push_back(i % 2 == 0);
  }
  assert(b1.size() == 1034);

  DynamicBitset b2;
  b2.push_back(true);
  assert(b2.size() == 1);
  assert(b2.popcnt() == 1);
  b2.push_back(false);
  assert(b2.size() == 2);
  assert(b2.popcnt() == 1);
  b2.pop_back();
  b2.pop_back();
  assert(b2.size() == 0);
  assert(b2.empty());

  DynamicBitset b3(512, false);
  assert(b3.size() == 512);
  assert(b3.popcnt() == 0);

  b3.resize(1024, true);
  assert(b3.size() == 1024);
  assert(b3.popcnt() == 512);
}

void DynamicBitsetTest::test_single_bit_access_functions() {
  DynamicBitset b1 = DynamicBitset(10, true);
  for (size_t i = 0; i < 10; ++i) {
    assert(b1.get(i) == true);
  }

  for (size_t i = 0; i < 10; ++i) {
    assert(b1.toggle(i) == true);
  }

  for (size_t i = 0; i < 10; ++i) {
    assert(b1.get(i) == false);
  }

  for (size_t i = 0; i < 999; ++i) {
    b1.push_back(true);
  }

  assert(b1.popcnt() == 999);
}

void DynamicBitsetTest::test_equality() {
  DynamicBitset b1 = DynamicBitset(10, true);
  DynamicBitset b2 = DynamicBitset(10, true);
  DynamicBitset b3 = DynamicBitset(10, false);
  DynamicBitset b4 = DynamicBitset(20, true);

  assert(b1 == b2);
  assert(b1 != b3);
  assert(b1 != b4);

  b1.clear();
  b4.clear();

  for (size_t i = 0; i < 823; ++i) {
    b1.push_back(i % 2 == 0);
    b4.push_back(i % 2 == 0);
  }

  assert(b1 == b4);
}

void DynamicBitsetTest::test_bitwise_operations() {
  DynamicBitset b1 = DynamicBitsetConverter::from_string("11010");
  DynamicBitset b2 = DynamicBitsetConverter::from_string("01011");

  DynamicBitset b3 = b1.clone_not();
  assert(b3 == DynamicBitsetConverter::from_string("00101"));

  DynamicBitset b4 = b1.clone_or(b2);
  assert(b4 == DynamicBitsetConverter::from_string("11011"));

  DynamicBitset b5 = b1.clone_and(b2);
  assert(b5 == DynamicBitsetConverter::from_string("01010"));

  DynamicBitset b6 = b1.clone_xor(b2);
  assert(b6 == DynamicBitsetConverter::from_string("10001"));

  DynamicBitset b7 = b1.clone_nor(b2);
  assert(b7 == DynamicBitsetConverter::from_string("00100"));

  DynamicBitset b8 = b1.clone_asymmetric_difference(b2);
  assert(b8 == DynamicBitsetConverter::from_string("10000"));
}

}  // namespace pmt::base::test