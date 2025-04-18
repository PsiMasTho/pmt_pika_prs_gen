#include "pmt/base/test/interval_set_test.hpp"

#include "pmt/base/dynamic_bitset_converter.hpp"
#include "pmt/base/interval_set.hpp"

#include <cassert>
#include <iostream>

namespace pmt::base::test {

namespace {

template <typename T_>
void debug_print(std::ostream& out_, IntervalSet<T_> const& interval_set_) {
  out_ << "IntervalSet (sizeof: " << sizeof(IntervalSet<T_>) << ", size: " << interval_set_.size() << ", capacity: " << interval_set_.capacity() << ")\n";
  for (size_t i = 0; i < interval_set_.size(); ++i) {
    Interval const interval = interval_set_.get(i);
    out_ << "[" << interval.get_lower() << ", " << interval.get_upper() << "]\n";
  }
  out_ << std::endl;
}
}  // namespace

void IntervalSetTest::run() {
  test_insert_1();
}

void IntervalSetTest::test_insert_1() {
  DynamicBitset bs1 = DynamicBitsetConverter::from_string("0001001100011100011000111101010101110011011010101000101111101011101010001110101");
  DynamicBitset bs2 = DynamicBitsetConverter::from_string("1001111111000111111100001111110000111111001101100111101010011111101010110010011");

  IntervalSet<int> is1 = DynamicBitsetConverter::to_interval_set<int>(bs1);
  IntervalSet<int> is2 = DynamicBitsetConverter::to_interval_set<int>(bs2);

  IntervalSet<int> is_overlap = is1.overlap(is2);
  DynamicBitset bs_overlap = DynamicBitsetConverter::from_interval_set(is_overlap);

  pmt_base_bitset_and(&bs1, &bs2);

  /* note: make sure the bitsets have no trailing zeros for this comparison to work properly */
  pmt_assert(pmt_base_bitset_eq(&bs_overlap, &bs1));

  pmt_base_interval_set_destruct(&is1);
  pmt_base_interval_set_destruct(&is2);
  pmt_base_interval_set_destruct(&is_overlap);
  pmt_base_bitset_destruct(&bs1);
  pmt_base_bitset_destruct(&bs2);
  pmt_base_bitset_destruct(&bs_overlap);

  bs1 = DynamicBitsetConverter::from_string("000100110100011101011101010100011101010101011011101010110001100101011101001110110101010100001111011100011");
  bs2 = DynamicBitsetConverter::from_string("101110101101010101010111111101010110101010111111111111111110001110111000111110011011101011100011101011111");

  is1 = DynamicBitsetConverter::to_interval_set(bs1);
  is2 = DynamicBitsetConverter::to_interval_set(bs2);

  is_overlap = pmt_base_interval_set_overlap(&is1, &is2, false);
  pmt_base_bitset_and(&bs1, &bs2);
  bs_overlap = pmt_base_bitset_from_interval_set(&is_overlap);
  pmt_assert(pmt_base_bitset_eq(&bs_overlap, &bs1));
}

}  // namespace pmt::base::test