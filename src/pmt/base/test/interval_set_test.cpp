#include "pmt/base/test/interval_set_test.hpp"

#include "pmt/base/bitset_converter.hpp"
#include "pmt/base/interval_set.hpp"

#include <cassert>
#include <random>
#include <unordered_set>
#include <optional>

namespace pmt::base::test {

namespace {

enum {
  RngSeed = 123,
};

static auto get_rng() -> std::mt19937& {
  static std::mt19937 rng(RngSeed);
  return rng;
}

// Testing the IntervalSet against std::unordered_set
template <typename T_>
struct TestSetPair {
  std::unordered_set<T_> _unordered_set;
  IntervalSet<T_> _interval_set;

  auto is_equal() const -> bool;
};

template <typename T_>
auto TestSetPair<T_>::is_equal() const -> bool {
  T_ const count = _interval_set.empty() ? 0 : _interval_set.highest();

  size_t found_in_interval_set_tally = 0;
  for (T_ i = 0; i <= count; ++i) {
    bool const found_in_unordered_set = _unordered_set.find(i) != _unordered_set.end();
    bool const found_in_interval_set = _interval_set.contains(i);

    found_in_interval_set_tally += found_in_interval_set ? 1 : 0;

    if (found_in_unordered_set != found_in_interval_set) {
      return false;
    }
  }

  return found_in_interval_set_tally == _unordered_set.size();
}

template <typename T_>
auto make_rng_filled_sets(size_t range_, float density_, size_t max_step_, std::optional<size_t> prefill_spacing_ = std::nullopt) -> TestSetPair<T_> {
  assert(density_ >= 0.0f && density_ <= 1.0f);

  TestSetPair<T_> ret;

  size_t const size = static_cast<size_t>(range_ * density_);
  size_t tally = 0;
  if (prefill_spacing_.has_value()) {
    for (size_t i = 0; i < range_ && tally < size; ++i) {
      if (i % prefill_spacing_.value() != 0) {
        continue;
      }

      tally += ret._unordered_set.insert(i).second;
      ret._interval_set.insert(Interval<T_>(i));
    }
  }

  std::uniform_int_distribution<T_> dist(0, range_ - 1);

  while (tally < size) {
    T_ const lower = dist(get_rng());
    size_t const step = std::min(std::min(max_step_, range_ - lower), size - tally);
    std::uniform_int_distribution<T_> dist2(lower, lower + step - 1);
    T_ const upper = dist2(get_rng());

    // Insert into unordered_set
    for (T_ i = lower; i <= upper; ++i) {
      tally += ret._unordered_set.insert(i).second;
    }

    // Insert into interval_set
    ret._interval_set.insert(Interval<T_>(lower, upper));
  }

  return ret;
}

template <std::integral T_>
void test_insert_impl() {
  static size_t const test_case_count = 50;

  for (size_t i = 0; i < test_case_count; ++i) {
    static size_t const range = std::min<size_t>(2048, std::numeric_limits<T_>::max());
    static float const density = 0.7f;
    static size_t const max_step = 16;
    static size_t const prefill_spacing = 8;

    TestSetPair<size_t> test_set_pair = make_rng_filled_sets<size_t>(range, density, max_step, prefill_spacing);

    assert(test_set_pair.is_equal());
  }
}

void equalize_bitset_sizes(Bitset& lhs_, Bitset& rhs_) {
  if (lhs_.size() > rhs_.size()) {
    lhs_.resize(rhs_.size());
  } else if (rhs_.size() > lhs_.size()) {
    rhs_.resize(lhs_.size());
  }
}

}  // namespace

void IntervalSetTest::run() {
  test_insert();
  test_and();
  test_erase();
  test_popcnt();
  test_asymmetric_difference();
  test_empty();
  test_copy();
}

void IntervalSetTest::test_insert() {
  test_insert_impl<int8_t>();
  test_insert_impl<uint8_t>();
  test_insert_impl<int16_t>();
  test_insert_impl<uint16_t>();
  test_insert_impl<int32_t>();
  test_insert_impl<uint32_t>();
  test_insert_impl<int64_t>();
  test_insert_impl<uint64_t>();
  test_insert_impl<uintptr_t>();
  test_insert_impl<uintmax_t>();
}

void IntervalSetTest::test_and() {
  static size_t const test_case_count = 50;

  for (size_t i = 0; i < test_case_count; ++i) {
    static size_t const range = 1000;
    static float const density = 0.6f;
    static size_t const max_step = 32;
    static size_t const prefill_spacing = 25;

    TestSetPair<size_t> test_set_pair = make_rng_filled_sets<size_t>(range, density, max_step, prefill_spacing);
    TestSetPair<size_t> test_set_pair2 = make_rng_filled_sets<size_t>(range, density, max_step, prefill_spacing);

    IntervalSet<size_t> const overlap_is = test_set_pair._interval_set.clone_and(test_set_pair2._interval_set);
    Bitset overlap_is_bs = BitsetConverter::from_interval_set(overlap_is);

    Bitset unordered_set_bs = BitsetConverter::from_unordered_set(test_set_pair._unordered_set);
    Bitset unordered_set_bs2 = BitsetConverter::from_unordered_set(test_set_pair2._unordered_set);

    equalize_bitset_sizes(unordered_set_bs2, unordered_set_bs);

    Bitset unordered_set_overlap_bs = unordered_set_bs.clone_and(unordered_set_bs2);

    // The bitsets should be of the same size before comparison, so shrink the larger one
    equalize_bitset_sizes(overlap_is_bs, unordered_set_overlap_bs);

    assert(overlap_is_bs == unordered_set_overlap_bs);
  }
}

void IntervalSetTest::test_erase() {
  static size_t const test_case_count = 50;

  for (size_t i = 0; i < test_case_count; ++i) {
    static size_t const range = 1000;
    static float const density = 0.8f;
    static size_t const max_step = 32;
    static size_t const prefill_spacing = 25;

    TestSetPair<size_t> test_set_pair = make_rng_filled_sets<size_t>(range, density, max_step, prefill_spacing);

    assert(test_set_pair.is_equal());

    float const density_after_erase = 0.2f;
    size_t const count_after_erase = static_cast<size_t>(test_set_pair._unordered_set.size() * density_after_erase);

    while (test_set_pair._unordered_set.size() > count_after_erase) {
      size_t const idx = std::uniform_int_distribution<size_t>(0, range - 1)(get_rng());
      size_t const step = std::uniform_int_distribution<size_t>(1, max_step)(get_rng());
      size_t const end = std::min(idx + step, range - 1);

      for (size_t j = idx; j <= end; ++j) {
        test_set_pair._unordered_set.erase(j);
      }

      test_set_pair._interval_set.erase(Interval<size_t>(idx, end));
    }

    assert(test_set_pair.is_equal());
  }
}

void IntervalSetTest::test_popcnt() {
  size_t const test_case_count = 32;

  for (size_t i = 0; i < test_case_count; ++i) {
    static size_t const range = 512;
    static float const density = 0.8f;
    static size_t const max_step = 32;
    static size_t const prefill_spacing = 26;

    TestSetPair<size_t> test_set_pair = make_rng_filled_sets<size_t>(range, density, max_step, prefill_spacing);

    assert(test_set_pair.is_equal());

    size_t const popcnt = test_set_pair._interval_set.popcnt();
    assert(popcnt == test_set_pair._unordered_set.size());
  }
}

void IntervalSetTest::test_asymmetric_difference() {
  size_t const test_case_count = 32;

  for (size_t i = 0; i < test_case_count; ++i) {
    static size_t const range = 512;
    static float const density = 0.8f;
    static size_t const max_step = 32;
    static size_t const prefill_spacing = 26;

    TestSetPair<size_t> test_set_pair1 = make_rng_filled_sets<size_t>(range, density, max_step, prefill_spacing);
    TestSetPair<size_t> test_set_pair2 = make_rng_filled_sets<size_t>(range, density, max_step, prefill_spacing);
    assert(test_set_pair1.is_equal());
    assert(test_set_pair2.is_equal());

    Bitset unordered_set_bs1 = BitsetConverter::from_unordered_set(test_set_pair1._unordered_set);
    Bitset unordered_set_bs2 = BitsetConverter::from_unordered_set(test_set_pair2._unordered_set);
    equalize_bitset_sizes(unordered_set_bs1, unordered_set_bs2);
    Bitset unordered_set_asymmetric_difference_bs = unordered_set_bs1.clone_asymmetric_difference(unordered_set_bs2);

    IntervalSet<size_t> const asymmetric_difference_is = test_set_pair1._interval_set.clone_asymmetric_difference(test_set_pair2._interval_set);
    Bitset asymmetric_difference_is_bs = BitsetConverter::from_interval_set(asymmetric_difference_is);

    equalize_bitset_sizes(unordered_set_asymmetric_difference_bs, asymmetric_difference_is_bs);

    assert(unordered_set_asymmetric_difference_bs == asymmetric_difference_is_bs);
  }
}

void IntervalSetTest::test_empty() {
  IntervalSet<size_t> is;
  assert(is.size() == 0);
  is.insert(Interval<size_t>(22, 44));
  assert(is.size() == 1);
  is.clear();
  assert(is.size() == 0);
}

void IntervalSetTest::test_copy() {
  IntervalSet<size_t> is;
  is.insert(Interval<size_t>(22, 44));
  is.insert(Interval<size_t>(55, 134));
  is.insert(Interval<size_t>(444, 445));
  is.insert(Interval<size_t>(2222, 44553));

  IntervalSet<size_t> is2 = is;
  assert(is == is2);
}

}  // namespace pmt::base::test