#pragma once

#include "pmt/parser/rt/state_machine_tables_base.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <unordered_map>
#include <unordered_set>

namespace pmt::parser::builder {

class StateMachineTables : public pmt::parser::rt::StateMachineTablesBase {
 // -$ Types / Constants $-
private:
 // -$ Data $-
 pmt::util::sm::ct::StateMachine _state_machine;
 std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset> _accepts_bitsets_cache;
 std::unordered_map<pmt::util::sm::SymbolKindType, std::unordered_map<pmt::util::sm::StateNrType, pmt::base::Bitset>> _transitions_cached;
 size_t _accept_count = 0;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit StateMachineTables(pmt::util::sm::ct::StateMachine state_machine_, std::unordered_set<pmt::util::sm::SymbolKindType> const& symbol_kinds_to_cache_transitions_for_);

 // --$ Inherited: pmt::util::sm::StateMachineTablesBase $--
 auto get_state_nr_next(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolKindType kind_, pmt::util::sm::SymbolValueType symbol_) const -> pmt::util::sm::StateNrType override;
 auto get_state_transitions(pmt::util::sm::StateNrType state_nr_, pmt::util::sm::SymbolKindType kind_) const -> pmt::base::Bitset::ChunkSpanConst override;
 auto get_state_accepts(pmt::util::sm::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;
 auto get_accept_count() const -> size_t override;

private:
 void fill_accepts_bitsets_cache();
 void fill_transitions_bitset_cache(pmt::util::sm::SymbolKindType kind_);
};

}  // namespace pmt::parser::builder