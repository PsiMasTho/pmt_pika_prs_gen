#include "pmt/parser/grammar/index_permutation_generator.hpp"

#include <algorithm>
#include <cassert>

namespace pmt::parser::grammar {
namespace {}

IndexPermutationGenerator::IndexPermutationGenerator()
 : _permutation()
 , _select()
 , _min_items{0}
 , _max_items{0}
 , _sequence_length{0}
 , _i{0}
 , _done(true) {
 // Default constructor initializes everything to empty and done
}

IndexPermutationGenerator::IndexPermutationGenerator(size_t min_items_, size_t max_items_, size_t sequence_length_)
 : _permutation()
 , _select()
 , _min_items{min_items_}
 , _max_items{max_items_}
 , _sequence_length{sequence_length_}
 , _i{_min_items}
 , _done(false) {
 assert(_min_items <= _max_items);
 assert(_max_items <= _sequence_length);

 init_select();
 init_permutation();
}

void IndexPermutationGenerator::advance() {
 if (_done) {
  return;
 }

 if (std::next_permutation(_permutation.begin(), _permutation.end())) {
  return;
 }

 if (std::prev_permutation(_select.begin(), _select.end())) {
  init_permutation();
  return;
 }

 if (_i++ < _max_items) {
  init_select();
  init_permutation();
  return;
 }

 _done = true;
 _permutation.clear();
}

auto IndexPermutationGenerator::get_permutation() const -> std::optional<std::span<size_t const>> {
 if (_done) {
  return std::nullopt;
 }
 return _permutation;
}

void IndexPermutationGenerator::init_select() {
 _select.clear();
 _select.resize(_sequence_length, false);
 std::fill(_select.begin(), _select.begin() + _i, true);
}

void IndexPermutationGenerator::init_permutation() {
 _permutation.clear();
 _permutation.reserve(_select.size());
 for (size_t i = 0; i < _select.size(); ++i) {
  if (!_select[i]) {
   continue;
  }
  _permutation.push_back(i);
 }
}

}  // namespace pmt::parser::grammar