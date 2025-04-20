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
};

};  // namespace pmt::base::test