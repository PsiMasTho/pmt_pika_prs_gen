#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/util/sm/ct/state.hpp"

#include <map>

namespace pmt::util::sm::ct {

class StateMachine {
private:
 // -$ Data $-
 std::map<StateNrType, State> _states;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 StateMachine() = default;
 StateMachine(StateMachine const&) = default;
 StateMachine(StateMachine&&) = default;
 virtual ~StateMachine() = default;
 // --$ Operators $--
 auto operator=(StateMachine const&) -> StateMachine& = default;
 auto operator=(StateMachine&&) noexcept -> StateMachine& = default;

 // --$ Other $--
 auto get_state(StateNrType state_nr_) -> State*;
 auto get_state(StateNrType state_nr_) const -> State const*;

 auto get_state_nrs() const -> pmt::base::IntervalSet<StateNrType>;

 auto get_or_create_state(StateNrType state_nr_) -> State&;
 auto get_unused_state_nr() const -> StateNrType;
 auto create_new_state() -> StateNrType;

 void remove_state(StateNrType state_nr_);

 auto get_state_count() const -> size_t;
};

}  // namespace pmt::util::sm::ct