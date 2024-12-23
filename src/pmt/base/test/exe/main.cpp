#include "pmt/base/test/algo_test.hpp"
#include "pmt/base/test/dynamic_bitset_test.hpp"

auto main() -> int {
  pmt::base::test::AlgoTest::run();
  pmt::base::test::DynamicBitsetTest::run();
}