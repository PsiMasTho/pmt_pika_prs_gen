// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/util/parsect/alphabet_limits.hpp"
#endif
// clang-format on

namespace pmt::util::parsect {

template <IsStateTag TAG_>
AlphabetLimits<TAG_>::AlphabetLimits(StateMachine<TAG_> const& state_machine_) {
  for (size_t i = 0; i < StateTraits<TAG_>::SymbolKindCount; ++i) {
    for (State::StateNrType j = 0; j < state_machine_.size(); ++j) {
      State<TAG_> const& state = *state_machine_.get_state(j);
      for (auto const& [symbol, state_nr] : state._symbol_transitions[i]) {
        if (!_limits[i].has_value()) {
          _limits[i].emplace(symbol, symbol);
        } else {
          _limits[i]->first = std::min(_limits[i]->first, symbol);
          _limits[i]->second = std::max(_limits[i]->second, symbol);
        }
      }
    }
  }
}

template <IsStateTag TAG_>
auto AlphabetLimits<TAG_>::get_limit(size_t transition_kind_) const -> std::optional<std::pair<StateBase::SymbolType, StateBase::SymbolType>> const& {
  return _limits[transition_kind_];
}

}  // namespace pmt::util::parsect