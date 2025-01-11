#pragma once

#include "pmt/util/parsect/state_machine.hpp"

#include <optional>

namespace pmt::util::parsect {

class AlphabetLimits {
 public:
  AlphabetLimits(StateMachine const& state_machine_);

  auto get_limit(Symbol::KindType kind_) const -> std::optional<std::pair<Symbol::ValueType, Symbol::ValueType>>;

  void refresh(StateMachine const& state_machine_);

 private:
  std::unordered_map<Symbol::KindType, std::pair<Symbol::ValueType, Symbol::ValueType>> _limits;
};

}  // namespace pmt::util::parsect
