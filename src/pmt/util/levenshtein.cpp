#include "pmt/util/levenshtein.hpp"

#include <algorithm>
#include <numeric>

using namespace std;

namespace pmt {

auto Levenshtein::distance(string_view lhs_, string_view rhs_) -> size_t {
  if (lhs_.size() > rhs_.size())
    return distance(rhs_, lhs_);

  size_t const min_size = lhs_.size();
  size_t const max_size = rhs_.size();
  _matrix.resize(min_size + 1);

  iota(_matrix.begin(), _matrix.end(), 0);

  for (size_t j = 1; j <= max_size; ++j) {
    size_t previous_diagonal = _matrix[0];
    ++_matrix[0];

    for (size_t i = 1; i <= min_size; ++i) {
      size_t const previous_diagonal_save = _matrix[i];
      if (lhs_[i - 1] == rhs_[j - 1])
        _matrix[i] = previous_diagonal;
      else
        _matrix[i] = min(min(_matrix[i - 1], _matrix[i]), previous_diagonal) + 1;

      previous_diagonal = previous_diagonal_save;
    }
  }

  return _matrix[min_size];
}

}  // namespace pmt