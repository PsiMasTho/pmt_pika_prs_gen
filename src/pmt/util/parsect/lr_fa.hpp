#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/util/parsect/lr_item.hpp"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace pmt::util::parsect {

class LrFa {
 public:
  using StateNrType = uint64_t;
  class Symbol;
  class State;

  std::unordered_map<StateNrType, State> _states;
};

class LrFa::Symbol : public pmt::base::Hashable<LrFa::Symbol> {
 public:
  enum Type : uint8_t {
    TerminalReference = 0,
    RuleReference = 1,
  };

  auto hash() const -> size_t;
  auto operator==(Symbol const& other_) const -> bool = default;
  auto operator<(Symbol const& other_) const -> bool;

  size_t _index : 63;
  Type _type : 1;
};

class LrFa::State {
 public:
  auto operator==(State const& other_) const -> bool = default;

  std::unordered_set<LrItem> _items;
  std::unordered_map<Symbol, StateNrType> _transitions;
};

}  // namespace pmt::util::parsect