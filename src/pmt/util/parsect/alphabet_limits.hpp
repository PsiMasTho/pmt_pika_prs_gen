#pragma once

#include "pmt/util/parsect/state_base.hpp"
#include "pmt/util/parsect/state_machine.hpp"
#include "pmt/util/parsect/state_tag.hpp"
#include "pmt/util/parsect/state_traits.hpp"

#include <array>
#include <optional>

namespace pmt::util::parsect {

template <IsStateTag TAG_>
class AlphabetLimits {
 public:
  AlphabetLimits(StateMachine<TAG_> const& state_machine_);

  auto get_limit(size_t transition_kind_) const -> std::optional<std::pair<StateBase::SymbolType, StateBase::SymbolType>> const&;

 private:
  std::array<std::optional<std::pair<StateBase::SymbolType, StateBase::SymbolType>>, StateTraits<TAG_>::SymbolKindCount> _limits;
};

}  // namespace pmt::util::parsect

#include "pmt/util/parsect/alphabet_limits-inl.hpp"