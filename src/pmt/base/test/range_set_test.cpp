#include "pmt/base/test/range_set_test.hpp"

#include "pmt/base/range_set.hpp"

#include <cassert>
#include <iostream>

namespace pmt::base::test {

namespace {

template <typename T_>
void debug_print(std::ostream& out_, RangeSet<T_> const& range_set_) {
  out_ << "RangeSet: ";
  for (size_t i = 0; i < range_set_.size(); ++i) {
    auto [lower, upper] = range_set_.get(i);
    out_ << "[" << lower << ", " << upper << "] ";
  }
  out_ << std::endl;
}
}  // namespace

void RangeSetTest::run() {
  test_insert_1();
}

void RangeSetTest::test_insert_1() {
  RangeSet<int> rs;
  rs.insert(1, 2);
  rs.insert(3, 4);

  rs.insert(5);

  rs.insert(10, 20);
  rs.insert(21, 30);

  debug_print(std::cout, rs);
}

}  // namespace pmt::base::test