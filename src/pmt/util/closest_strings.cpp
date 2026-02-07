#include "pmt/util/closest_strings.hpp"

#include <algorithm>

namespace pmt::util {

ClosestStrings::ClosestStrings(std::string query_, size_t max_distance_, size_t max_candidates_)
 : _query(std::move(query_))
 , _max_distance(max_distance_)
 , _max_candidates(max_candidates_) {
}

void ClosestStrings::push(std::string candidate_) {
 size_t const distance = _lev.distance(_query, candidate_);

 // (distance too high) OR (full and not better than current worst)
 if ((distance > _max_distance) || (_distances.size() == _max_candidates && distance >= _distances.back())) {
  return;
 }

 // full but we need to insert
 if (_distances.size() == _max_candidates) {
  _candidates.pop_back();
  _distances.pop_back();
  _truncated = true;
 }

 // not full, insert in sorted order
 auto const itr = std::lower_bound(_distances.begin(), _distances.end(), distance);
 auto const idx = std::distance(_distances.begin(), itr);

 _distances.insert(itr, distance);
 _candidates.emplace(_candidates.begin() + idx, std::move(candidate_));
}

auto ClosestStrings::query() const -> std::string const& {
 return _query;
}

auto ClosestStrings::candidates() const -> std::span<std::string const> {
 return _candidates;
}

auto ClosestStrings::truncated() const -> bool {
 return _truncated;
}

}  // namespace pmt::util