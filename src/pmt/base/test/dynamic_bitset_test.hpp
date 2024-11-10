#pragma once

namespace pmt::base::test {

class DynamicBitsetTest {
 public:
  static void run();

 private:
  static void test_lifetime_functions();
  static void test_capacity_functions();
  static void test_single_bit_access_functions();
  static void test_equality();
  static void test_bitwise_operations();
};

};  // namespace pmt::base::test