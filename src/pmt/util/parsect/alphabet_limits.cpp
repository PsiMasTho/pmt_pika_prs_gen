#include "pmt/util/parsect/alphabet_limits.hpp"

namespace pmt::util::parsect {

AlphabetLimits::AlphabetLimits(StateMachine const& state_machine_) {
  refresh(state_machine_);
}

auto AlphabetLimits::get_limit(Symbol::KindType kind_) const -> std::optional<std::pair<Symbol::ValueType, Symbol::ValueType>> {
  auto const it = _limits.find(kind_);
  return it != _limits.end() ? std::make_optional(it->second) : std::nullopt;
}

void AlphabetLimits::refresh(StateMachine const& state_machine_) {
  _limits.clear();

  for (State const& state : state_machine_.get_states()) {
    for (Symbol::KindType const kind : state.get_symbol_kinds()) {
      auto& [min, max] = _limits[kind];

      min = state.get_symbols(kind).front()._value;
      max = state.get_symbols(kind).back()._value;
    }
  }
}

}  // namespace pmt::util::parsect