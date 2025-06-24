#include "pmt/base/test/bitset_test.hpp"

#include "pmt/base/bitset.hpp"
#include "pmt/base/bitset_converter.hpp"

#include <cassert>

namespace pmt::base::test {

void BitsetTest::run() {
 test_lifetime_functions();
 test_capacity_functions();
 test_single_bit_access_functions();
 test_equality();
 test_bitwise_operations();
 test_countl();
 test_countr();
 test_swap();
 test_exchange();
 test_any();
 test_none();
 test_all();
}

void BitsetTest::test_lifetime_functions() {
 Bitset b1 = Bitset(10, true);
 Bitset b2 = Bitset(b1);
 Bitset b3 = Bitset(20, false);

 assert(b1.size() == 10);
 assert(b2.size() == 10);
 assert(b3.size() == 20);

 Bitset b4 = std::move(b1);
 assert(b4.size() == 10);

 b1 = b2;
 assert(b1.size() == 10);
 assert(b2.size() == 10);

 b1 = std::move(b2);
 assert(b1.size() == 10);
}

void BitsetTest::test_capacity_functions() {
 Bitset b1 = Bitset(10, true);
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

 Bitset b2;
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

 Bitset b3(512, false);
 assert(b3.size() == 512);
 assert(b3.popcnt() == 0);

 b3.resize(1024, true);
 assert(b3.size() == 1024);
 assert(b3.popcnt() == 512);
}

void BitsetTest::test_single_bit_access_functions() {
 Bitset b1 = Bitset(10, true);
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

void BitsetTest::test_equality() {
 Bitset b1 = Bitset(10, true);
 Bitset b2 = Bitset(10, true);
 Bitset b3 = Bitset(10, false);
 Bitset b4 = Bitset(20, true);

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

void BitsetTest::test_bitwise_operations() {
 Bitset b1 = BitsetConverter::from_string("11010");
 Bitset b2 = BitsetConverter::from_string("01011");

 Bitset b3 = b1.clone_not();
 assert(b3 == BitsetConverter::from_string("00101"));

 Bitset b4 = b1.clone_or(b2);
 assert(b4 == BitsetConverter::from_string("11011"));

 Bitset b5 = b1.clone_and(b2);
 assert(b5 == BitsetConverter::from_string("01010"));

 Bitset b6 = b1.clone_xor(b2);
 assert(b6 == BitsetConverter::from_string("10001"));

 Bitset b7 = b1.clone_nor(b2);
 assert(b7 == BitsetConverter::from_string("00100"));

 Bitset b8 = b1.clone_asymmetric_difference(b2);
 assert(b8 == BitsetConverter::from_string("10000"));
}

void BitsetTest::test_countl() {
 static size_t const SIZE = 1234;

 // Test countl true
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, false);
  for (size_t j = 0; j < i; ++j) {
   b1.set(j, true);
  }

  assert(b1.countl(true) == i);
 }

 // Test countl false
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, true);
  for (size_t j = 0; j < i; ++j) {
   b1.set(j, false);
  }

  assert(b1.countl(false) == i);
 }
}

void BitsetTest::test_countr() {
 static size_t const SIZE = 1234;

 // Test countr true
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, false);
  for (size_t j = 0; j < i; ++j) {
   b1.set(SIZE - 1 - j, true);
  }

  assert(b1.countr(true) == i);
 }

 // Test countr false
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, true);
  for (size_t j = 0; j < i; ++j) {
   b1.set(SIZE - 1 - j, false);
  }

  assert(b1.countr(false) == i);
 }
}

void BitsetTest::test_swap() {
 Bitset b1 = BitsetConverter::from_string("11010");
 b1.swap(0, 1);
 assert(b1 == BitsetConverter::from_string("11010"));
 b1.swap(1, 2);
 assert(b1 == BitsetConverter::from_string("10110"));
 b1.swap(2, 3);
 assert(b1 == BitsetConverter::from_string("10110"));
 b1.swap(3, 4);
 assert(b1 == BitsetConverter::from_string("10101"));
 b1.swap(4, 0);
 assert(b1 == BitsetConverter::from_string("10101"));
}

void BitsetTest::test_exchange() {
 Bitset b1 = BitsetConverter::from_string("11010");
 assert(b1.exchange(0, true) == true);
 assert(b1 == BitsetConverter::from_string("11010"));
 assert(b1.exchange(1, false) == true);
 assert(b1 == BitsetConverter::from_string("10010"));
 assert(b1.exchange(2, true) == false);
 assert(b1 == BitsetConverter::from_string("10110"));
 assert(b1.exchange(3, false) == true);
 assert(b1 == BitsetConverter::from_string("10100"));
 assert(b1.exchange(4, true) == false);
 assert(b1 == BitsetConverter::from_string("10101"));
}

void BitsetTest::test_any() {
 static size_t const SIZE = 1234;

 // positive test
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, false);
  b1.set(i / 7, true);
  assert(b1.any());
 }

 // negative test
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, false);
  assert(!b1.any());
 }
}

void BitsetTest::test_none() {
 static size_t const SIZE = 1234;

 // positive test
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, false);
  assert(b1.none());
 }

 // negative test
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, false);
  b1.set(i / 7, true);
  assert(!b1.none());
 }
}

void BitsetTest::test_all() {
 static size_t const SIZE = 1234;

 // positive test
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, true);
  assert(b1.all());
 }

 // negative test
 for (size_t i = 0; i < SIZE; ++i) {
  Bitset b1 = Bitset(SIZE, true);
  b1.set(i / 7, false);
  assert(!b1.all());
 }
}

}  // namespace pmt::base::test