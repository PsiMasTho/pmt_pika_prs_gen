#include "pmt/util/parsect/flat_grammar.hpp"

#include "pmt/base/overloaded.hpp"

#include <algorithm>
#include <cstddef>

namespace pmt::util::parsect {
using namespace pmt::base;

auto FlatGrammar::Production::operator<(Production const& other_) const -> bool {
  if (_rule_index < other_._rule_index) {
    return true;
  }
  if (_rule_index > other_._rule_index) {
    return false;
  }

  return std::lexicographical_compare(_sequence.begin(), _sequence.end(), other_._sequence.begin(), other_._sequence.end());
}

void FlatGrammar::add_production(Production production_) {
  auto const itr = std::upper_bound(_productions.begin(), _productions.end(), production_);
  if (itr != _productions.end() && *itr == production_) {
    return;
  }

  _productions.insert(itr, std::move(production_));
}

auto FlatGrammar::find_productions(size_t rule_index_) const -> std::pair<size_t, size_t> {
  // clang-format off
  auto const cmp = Overloaded{
   [](Production const& production_, size_t rule_index_) -> bool {
    return production_._rule_index < rule_index_;
   },
   [](size_t rule_index_, Production const& production_) -> bool {
    return rule_index_ < production_._rule_index;
   },
   [](auto const& lhs_, auto const& rhs_) -> bool {
    return lhs_ < rhs_;
   }};
  // clang-format on
  auto const range = std::equal_range(_productions.begin(), _productions.end(), rule_index_, cmp);
  return {std::distance(_productions.begin(), range.first), std::distance(range.first, range.second)};
}

auto FlatGrammar::get_production(size_t production_index_) const -> Production const& {
  return _productions[production_index_];
}

auto FlatGrammar::get_closure(std::unordered_set<LrItem> const& items_) const -> std::unordered_set<LrItem> {
  bool changed = true;
  std::unordered_set<LrItem> closure = items_;
  while (changed) {
    changed = false;
    for (LrItem const item : closure) {
      if (item._dot_position == get_production(item._production_index)._sequence.size()) {
        continue;
      }

      LrFa::Symbol const symbol = get_production(item._production_index)._sequence[item._dot_position];
      if (symbol._type != LrFa::Symbol::RuleReference) {
        continue;
      }

      auto const [start, count] = find_productions(symbol._index);
      for (size_t i = start; i < start + count; ++i) {
        LrItem const new_item = {i, 0};
        if (closure.insert(new_item).second) {
          changed = true;
        }
      }
    }
  }
  return closure;
}

auto FlatGrammar::get_goto(std::unordered_set<LrItem> const& items_, LrFa::Symbol symbol_) const -> std::unordered_set<LrItem> {
  std::unordered_set<LrItem> goto_set;
  for (LrItem const item : items_) {
    if (item._dot_position == get_production(item._production_index)._sequence.size()) {
      continue;
    }

    if (get_production(item._production_index)._sequence[item._dot_position] == symbol_) {
      goto_set.insert({item._production_index, item._dot_position + 1});
    }
  }
  return get_closure(goto_set);
}

}  // namespace pmt::util::parsect