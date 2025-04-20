#pragma once

namespace pmt::base::test {

class IntervalSetTest {
 public:
  static void run();

 private:
  static void test_insert();
  static void test_overlap();
  static void test_erase();
};

};  // namespace pmt::base::test