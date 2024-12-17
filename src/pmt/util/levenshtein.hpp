#pragma once

#include <string_view>
#include <vector>

namespace pmt {

class Levenshtein {
 public:
  [[nodiscard]] auto distance(std::string_view lhs_, std::string_view rhs_) -> std::size_t;

 private:
  std::vector<std::size_t> _matrix;
};

}  // namespace pmt