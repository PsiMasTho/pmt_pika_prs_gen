#pragma once

#include "pmt/builder/state.hpp"
#include "pmt/container/multi_vector.hpp"
#include "pmt/rt/primitives.hpp"

namespace pmt::builder {

class StateMachine {
private:
 // -$ Data $-
 pmt::container::MultiVector<pmt::rt::StateNrType, State> _states;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 StateMachine() = default;
 StateMachine(StateMachine const&) = default;
 StateMachine(StateMachine&&) = default;

 // --$ Operators $--
 auto operator=(StateMachine const&) -> StateMachine& = default;
 auto operator=(StateMachine&&) noexcept -> StateMachine& = default;

 // --$ Other $--
 auto get_state(pmt::rt::StateNrType state_nr_) -> State*;
 auto get_state(pmt::rt::StateNrType state_nr_) const -> State const*;

 auto get_state_nrs() const -> std::span<pmt::rt::StateNrType const>;

 auto get_or_create_state(pmt::rt::StateNrType state_nr_) -> State&;
 auto get_unused_state_nr() const -> pmt::rt::StateNrType;
 auto create_new_state() -> pmt::rt::StateNrType;

 void remove_state(pmt::rt::StateNrType state_nr_);

 auto get_state_count() const -> size_t;
};

}  // namespace pmt::builder