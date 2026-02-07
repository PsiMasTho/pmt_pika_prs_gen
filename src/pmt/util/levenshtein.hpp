#pragma once

#include <string_view>
#include <vector>

namespace pmt::util {

class Levenshtein {
 // -$ Data $-
 std::vector<size_t> _matrix;

public:
 // -$ Functions $-
 // --$ Other $--
 [[nodiscard]] auto distance(std::string_view lhs_, std::string_view rhs_) -> size_t;
};

}  // namespace pmt::util