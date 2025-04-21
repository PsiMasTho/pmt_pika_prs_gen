#pragma once

namespace pmt::base::test {

class IntervalSetTest {
 public:
  static void run();

 private:
  static void test_insert();
  static void test_and();
  static void test_erase();
  static void test_popcnt();
  static void test_asymmetric_difference();
  static void test_empty();
  static void test_copy();
};

};  // namespace pmt::base::test