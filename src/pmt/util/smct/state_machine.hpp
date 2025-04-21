#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/util/smct/state.hpp"
#include "pmt/util/smrt/state_machine_tables_base.hpp"

#include <map>

namespace pmt::util::smct {

class StateMachine : public pmt::util::smrt::StateMachineTablesBase {
 private:
  // -$ Data $-
  std::map<pmt::util::smrt::StateNrType, State> _states;

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

  // --$ Inherited: pmt::util::smrt::StateMachineTablesBase $--
  auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType final;
  auto get_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst final;
  auto get_accept_count() const -> size_t final;

  // --$ Other $--
  auto get_state(pmt::util::smrt::StateNrType state_nr_) -> State*;
  auto get_state(pmt::util::smrt::StateNrType state_nr_) const -> State const*;

  auto get_state_nrs() const -> pmt::base::IntervalSet<pmt::util::smrt::StateNrType>;

  auto get_or_create_state(pmt::util::smrt::StateNrType state_nr_) -> State&;
  auto get_unused_state_nr() const -> pmt::util::smrt::StateNrType;
  auto create_new_state() -> pmt::util::smrt::StateNrType;

  void remove_state(pmt::util::smrt::StateNrType state_nr_);

  auto get_state_count() const -> size_t;

  void debug_dump();  // REMOVE ME
};

}  // namespace pmt::util::smct