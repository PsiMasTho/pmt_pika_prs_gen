#pragma once

#include "pmt/parser/bytecode.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/parser/rt/program_base.hpp"

namespace pmt::parser::builder {

class Program : public pmt::parser::rt::ProgramBase {
public:
 // -$ Types / Constants $-
 using LitSeqType = std::vector<pmt::base::IntervalSet<SymbolType>>;

private:
 // -$ Data $-
 std::vector<LitSeqType> _lit_seq_table;
 std::vector<pmt::parser::rt::RuleInfo> _rule_info_table;
 std::vector<Instruction> _instructions;
 ProgramCounterType _entry_pc = 0;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 // --$ Inherited: pmt::parser::rt::ProgramBase $--
 [[nodiscard]] auto fetch_instruction(ProgramCounterType pc_) const -> Instruction override;
 [[nodiscard]] auto fetch_rule_info(ArgType rule_info_id_) const -> pmt::parser::rt::RuleInfo override;

 [[nodiscard]] auto lit_seq_match_at(ArgType lit_seq_id_, size_t pos_, SymbolType sym_) const -> bool override;
 [[nodiscard]] auto get_lit_seq_length(ArgType lit_seq_id_) const -> size_t override;

 [[nodiscard]] auto get_entry_pc() const -> ProgramCounterType override;
 [[nodiscard]] auto get_max_pc() const -> ProgramCounterType override;

 // --$ Other $--
 void set_entry_pc(ProgramCounterType pc_);
 auto push_instruction(Instruction instruction_) -> ProgramCounterType;
 auto add_rule_info(pmt::parser::rt::RuleInfo rule_info_) -> ArgType;
 auto add_lit_seq(LitSeqType lit_seq_) -> ArgType;
};
}  // namespace pmt::parser::builder
