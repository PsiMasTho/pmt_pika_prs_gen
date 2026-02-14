#pragma once

#include "pmt/rt/primitives.hpp"
#include "pmt/rt/state_machine_tables_base.hpp"
#include "pmt/sm/state_machine.hpp"

namespace pmt::builder {

class StateMachineTables : public pmt::rt::StateMachineTablesBase {
 // -$ Types / Constants $-
private:
 // -$ Data $-
 pmt::sm::StateMachine _state_machine;
 std::vector<std::vector<pmt::rt::IdType>> _state_final_ids_cache;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit StateMachineTables(pmt::sm::StateMachine state_machine_ = {});

 // --$ Inherited: pmt::util::sm::StateMachineTablesBase $--
 auto get_state_nr_start() const -> pmt::rt::StateNrType override;
 auto get_state_nr_invalid() const -> pmt::rt::StateNrType override;

 auto get_state_nr_next(pmt::rt::StateNrType state_nr_, pmt::rt::SymbolType symbol_) const -> pmt::rt::StateNrType override;

 auto get_state_final_id_count(pmt::rt::StateNrType state_nr_) const -> size_t override;
 auto get_state_final_id(pmt::rt::StateNrType state_nr_, size_t index_) const -> pmt::rt::IdType override;

 // --$ Other $--
 auto get_state_machine() const -> pmt::sm::StateMachine const&;

private:
 void init_cache();
};

}  // namespace pmt::builder
