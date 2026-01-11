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
 std::vector<pmt::base::Bitset> _accepts_bitsets_cache;  // Indexed by StateNrType
 size_t _max_accept_cached = 0;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit StateMachineTables(pmt::sm::StateMachine state_machine_ = {});

 // --$ Inherited: pmt::util::sm::StateMachineTablesBase $--
 auto get_state_nr_next(pmt::sm::StateNrType state_nr_, pmt::sm::SymbolType symbol_) const -> pmt::sm::StateNrType override;
 auto get_state_accepts(pmt::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;

 // --$ Other $--
 auto get_state_machine() const -> pmt::sm::StateMachine const&;
 auto get_max_accept() const -> pmt::sm::AcceptsIndexType;

private:
 void init_cache();
};

}  // namespace pmt::parser::builder