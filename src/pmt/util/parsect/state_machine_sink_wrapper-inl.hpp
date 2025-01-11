// clang-format off
#ifdef __INTELLISENSE__
 #include "pmt/util/parsect/state_machine_sink_wrapper.hpp"
#endif
// clang-format on

namespace pmt::util::parsect {

template <IsStateTag TAG_>
StateMachineSinkWrapper<TAG_>::StateMachineSinkWrapper(StateMachine<TAG_> const& state_machine_, AlphabetLimits<TAG_> const& alphabet_limits_)
 : _state_machine(state_machine_)
 , _has_sink(false) {
  refresh(alphabet_limits_);
}

template <IsStateTag TAG_>
auto StateMachineSinkWrapper<TAG_>::get_state_machine() const -> StateMachine<TAG_> const& {
  return _state_machine;
}

template <IsStateTag TAG_>
auto StateMachineSinkWrapper<TAG_>::get_state_nr_next(State::StateNrType state_nr_, size_t symbol_kind_, StateBase::SymbolType symbol_) const -> State::StateNrType {
  State<TAG_> const* state = _state_machine.get_state(state_nr_);

  if (state == nullptr) {
    return _state_machine.size();
  }

  auto const itr = state->_symbol_transitions[symbol_kind_].find(symbol_);
  return itr != state->_symbol_transitions[symbol_kind_].end() ? itr->second : _state_machine.size();
}

template <IsStateTag TAG_>
auto StateMachineSinkWrapper<TAG_>::get_state_nr_sink() const -> std::optional<State::StateNrType> {
  return _has_sink ? std::optional<State::StateNrType>(_state_machine.size()) : std::nullopt;
}

template <IsStateTag TAG_>
auto StateMachineSinkWrapper<TAG_>::size() const -> size_t {
  return _state_machine.size() + (_has_sink ? 1 : 0);
}

template <IsStateTag TAG_>
void StateMachineSinkWrapper<TAG_>::refresh(AlphabetLimits<TAG_> const& alphabet_limits_) {
  _has_sink = false;

  for (auto const& [state_nr, state] : _state_machine._states) {
    for (size_t i = 0; i < StateTraits<TAG_>::SymbolKindCount; ++i) {
      std::optional<std::pair<StateBase::SymbolType, State::StateNrType>> const& limits = alphabet_limits_.get_limit(i);
      if (!limits.has_value()) {
        continue;
      }
      for (StateBase::SymbolType j = limits->first; j <= limits->second; ++j) {
        if (!state._symbol_transitions[i].contains(j)) {
          _has_sink = true;
          return;
        }
      }
    }
  }
}

}  // namespace pmt::util::parsect