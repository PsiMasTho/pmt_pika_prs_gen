#include "pmt/base/test/algo_test.hpp"
#include "pmt/base/test/bitset_test.hpp"
#include "pmt/base/test/interval_map_test.hpp"
#include "pmt/base/test/interval_set_test.hpp"

auto main() -> int {
  pmt::base::test::AlgoTest::run();
  pmt::base::test::BitsetTest::run();
  pmt::base::test::IntervalMapTest::run();
  pmt::base::test::IntervalSetTest::run();
}