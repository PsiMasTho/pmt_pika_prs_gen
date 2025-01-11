#pragma once

#include "alphabet_limits.hpp"
#include "pmt/util/parsect/state_machine.hpp"
#include "pmt/util/parsect/state_tag.hpp"

#include <optional>

namespace pmt::util::parsect {

template <IsStateTag TAG_>
class StateMachineSinkWrapper {
 public:
  StateMachineSinkWrapper(StateMachine<TAG_> const& state_machine_, AlphabetLimits<TAG_> const& alphabet_limits_);

  auto get_state_machine() const -> StateMachine<TAG_> const&;

  auto get_state_nr_next(State::StateNrType state_nr_, size_t symbol_kind_, StateBase::SymbolType symbol_) const -> State::StateNrType;
  auto get_state_nr_sink() const -> std::optional<State::StateNrType>;
  auto size() const -> size_t;

  void refresh(AlphabetLimits<TAG_> const& alphabet_limits_);

 private:
  StateMachine<TAG_> _state_machine;
  bool _has_sink;
};

}  // namespace pmt::util::parsect

#include "pmt/util/parsect/state_machine_sink_wrapper-inl.hpp"