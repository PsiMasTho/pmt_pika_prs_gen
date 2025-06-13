#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace pmt::parser::grammar {

class IndexPermutationGenerator {
  // -$ Data $-
  std::vector<bool> _select;
  std::vector<size_t> _permutation;

  size_t _min_items;
  size_t _max_items;
  size_t _sequence_length;
  size_t _i;
  bool _done : 1;

 public:
  IndexPermutationGenerator();
  explicit IndexPermutationGenerator(size_t min_items_, size_t max_items_, size_t sequence_length_);

  void advance();
  auto get_permutation() const -> std::optional<std::span<size_t const>>;

 private:
  void init_select();
  void init_permutation();
};

}  // namespace pmt::parser::grammar