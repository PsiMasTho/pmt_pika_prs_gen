#pragma once

#include "pmt/parser/rt/state_machine_tables_base.hpp"
#include "pmt/sm/primitives.hpp"
#include "pmt/sm/state_machine.hpp"

namespace pmt::parser::builder {

class StateMachineTables : public pmt::parser::rt::StateMachineTablesBase {
 // -$ Types / Constants $-
private:
 // -$ Data $-
 pmt::sm::StateMachine _state_machine;
 std::vector<std::vector<pmt::sm::AcceptsIndexType>> _state_accepts_cache;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit StateMachineTables(pmt::sm::StateMachine state_machine_ = {});

 // --$ Inherited: pmt::util::sm::StateMachineTablesBase $--
 auto get_state_nr_next(pmt::sm::StateNrType state_nr_, pmt::sm::SymbolType symbol_) const -> pmt::sm::StateNrType override;

 auto get_state_accept_count(pmt::sm::StateNrType state_nr_) const -> size_t override;
 auto get_state_accept_at(pmt::sm::StateNrType state_nr_, size_t index_) const -> pmt::sm::AcceptsIndexType override;

 // --$ Other $--
 auto get_state_machine() const -> pmt::sm::StateMachine const&;

private:
 void init_cache();
};

}  // namespace pmt::parser::builder
