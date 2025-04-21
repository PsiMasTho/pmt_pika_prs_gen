#pragma once

namespace pmt::base::test {

class IntervalMapTest {
 public:
  static void run();

 private:
  static void test_insert();
  static void test_erase();
  static void test_copy();
};

};  // namespace pmt::base::test