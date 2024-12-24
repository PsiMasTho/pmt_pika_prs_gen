#pragma once

#include "pmt/util/parsect/lr_fa.hpp"

#include <cstddef>
#include <unordered_set>
#include <vector>

namespace pmt::util::parsect {

class FlatGrammar {
 public:
  using Sequence = std::vector<LrFa::Symbol>;

  class Production {
   public:
    auto operator==(Production const& other_) const -> bool = default;
    auto operator<(Production const& other_) const -> bool;

    size_t _rule_index;
    Sequence _sequence;
  };

  void add_production(Production production_);
  auto find_productions(size_t rule_index_) const -> std::pair<size_t, size_t>;  // <start, count>
  auto get_production(size_t production_index_) const -> Production const&;

  auto get_closure(std::unordered_set<LrItem> const& items_) const -> std::unordered_set<LrItem>;
  auto get_goto(std::unordered_set<LrItem> const& items_, LrFa::Symbol symbol_) const -> std::unordered_set<LrItem>;

 private:
  std::vector<Production> _productions;
};

}  // namespace pmt::util::parsect
