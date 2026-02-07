#pragma once

#include "pmt/util/levenshtein.hpp"

#include <span>
#include <string>
#include <vector>

namespace pmt::util {

class ClosestStrings {
 // -$ Data $-
 std::string _query;
 Levenshtein _lev;
 std::vector<std::string> _candidates;
 std::vector<size_t> _distances;
 size_t _max_distance = 0;
 size_t _max_candidates = 0;
 bool _truncated = false;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 ClosestStrings(std::string query_, size_t max_distance_, size_t max_candidates_);

 // --$ Other $--
 void push(std::string candidate_);
 auto query() const -> std::string const&;
 auto candidates() const -> std::span<std::string const>;
 auto truncated() const -> bool;
};

}  // namespace pmt::util