#pragma once

namespace pmt::base::test {

class BitsetTest {
public:
 static void run();

private:
 static void test_lifetime_functions();
 static void test_capacity_functions();
 static void test_single_bit_access_functions();
 static void test_equality();
 static void test_bitwise_operations();
 static void test_countl();
 static void test_countr();
 static void test_swap();
 static void test_exchange();
 static void test_any();
 static void test_none();
 static void test_all();
 static void test_for_each_bit();
};

};  // namespace pmt::base::test