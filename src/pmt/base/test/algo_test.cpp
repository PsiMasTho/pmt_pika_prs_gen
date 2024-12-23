#include "pmt/base/test/algo_test.hpp"

#include "pmt/base/algo.hpp"

#include <algorithm>
#include <cassert>
#include <random>

namespace pmt::base::test {

void AlgoTest::run() {
  test_apply_permutation();
}

void AlgoTest::test_apply_permutation() {
  static size_t const ITERATIONS = 256;
  static size_t const SIZE = 256;

  static size_t const SEED = 42;

  std::mt19937 gen(SEED);

  for (size_t i = 0; i < ITERATIONS; ++i) {
    std::vector<int> indices;
    std::generate_n(std::back_inserter(indices), SIZE, [i = 0]() mutable { return i++; });
    std::vector<int> data = indices;

    std::ranges::shuffle(indices, gen);

    apply_permutation(data.begin(), data.end(), indices.begin());

    assert(std::equal(data.begin(), data.end(), indices.begin()));
  }
}

}  // namespace pmt::base::test