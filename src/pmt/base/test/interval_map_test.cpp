#include "pmt/base/test/interval_map_test.hpp"

#include "pmt/base/interval_map.hpp"

#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

namespace pmt::base::test {

namespace {

enum {
  RngSeed = 123,
};

static auto get_rng() -> std::mt19937& {
  static std::mt19937 rng(RngSeed);
  return rng;
}

// Testing the IntervalMap against std::unordered_map
template <typename T_>
struct TestMapPair {
  std::unordered_map<T_, std::string> _unordered_map;
  IntervalMap<T_, std::string> _interval_map;

  auto is_equal() const -> bool;
};

template <typename T_>
auto TestMapPair<T_>::is_equal() const -> bool {
  T_ const count = _interval_map.empty() ? 0 : _interval_map.highest();

  size_t found_in_interval_map_tally = 0;
  for (T_ i = 0; i <= count; ++i) {
    auto const itr_unordered_map = _unordered_map.find(i);
    std::string const* val_interval_map = _interval_map.find(i);

    found_in_interval_map_tally += (val_interval_map != nullptr) ? 1 : 0;

    if ((itr_unordered_map != _unordered_map.end()) != (val_interval_map != nullptr)) {
      return false;
    }

    if (val_interval_map != nullptr && itr_unordered_map->second != *val_interval_map) {
      return false;
    }
  }

  return found_in_interval_map_tally == _unordered_map.size();
}

template <typename T_>
auto make_rng_filled_maps(size_t range_, float density_, size_t max_step_, size_t value_max_, std::optional<size_t> prefill_spacing_ = std::nullopt) -> TestMapPair<T_> {
  assert(density_ >= 0.0f && density_ <= 1.0f);

  static std::string const value_prefix = "Value_";

  TestMapPair<T_> ret;

  std::uniform_int_distribution<size_t> value_dist(0, value_max_);

  size_t const size = static_cast<size_t>(range_ * density_);
  size_t tally = 0;
  if (prefill_spacing_.has_value()) {
    for (size_t i = 0; i < range_ && tally < size; ++i) {
      if (i % prefill_spacing_.value() != 0) {
        continue;
      }

      std::string const value = value_prefix + std::to_string(value_dist(get_rng()));

      tally += ret._unordered_map.insert_or_assign(i, value).second;
      ret._interval_map.insert(Interval<T_>(i), value);
    }
  }

  std::uniform_int_distribution<T_> dist(0, range_ - 1);

  while (tally < size) {
    T_ const lower = dist(get_rng());
    size_t const step = std::min(std::min(max_step_, range_ - lower), size - tally);
    std::uniform_int_distribution<T_> dist2(lower, lower + step - 1);
    T_ const upper = dist2(get_rng());

    std::string const value = value_prefix + std::to_string(value_dist(get_rng()));

    // Insert into unordered_map
    for (T_ i = lower; i <= upper; ++i) {
      tally += ret._unordered_map.insert_or_assign(i, value).second;
    }

    // Insert into interval_set
    ret._interval_map.insert(Interval<T_>(lower, upper), value);
  }

  return ret;
}

template <typename T_, typename U_>
void debug_print(IntervalMap<T_, U_> const& interval_map_) {
  std::cout << "Interval map: (sizeof: " << sizeof(interval_map_) << ", size: " << interval_map_.size() << ", capacity: " << interval_map_.capacity() << ")\n";
  std::cout << "Intervals:\n";
  for (size_t i = 0; i < interval_map_.size(); ++i) {
    auto const entry = interval_map_.get_by_index(i);
    std::cout << "Interval: [" << entry._interval.get_lower() << ", " << entry._interval.get_upper() << "], Value: " << entry._value << "\n";
  }
}

}  // namespace

void IntervalMapTest::run() {
  test_insert();
}

void IntervalMapTest::test_insert() {
  static size_t const TEST_CASE_COUNT = 50;

  for (size_t i = 0; i < TEST_CASE_COUNT; ++i) {
    static size_t const range = 2048;
    static float const density = 0.7f;
    static size_t const max_step = 16;
    static size_t const value_max = 6;
    static size_t const prefill_spacing = 8;

    TestMapPair<size_t> test_map_pair = make_rng_filled_maps<size_t>(range, density, max_step, value_max, prefill_spacing);

    assert(test_map_pair.is_equal());
  }
}

}  // namespace pmt::base::test