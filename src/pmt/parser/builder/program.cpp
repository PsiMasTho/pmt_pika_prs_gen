#include "pmt/parser/builder/program.hpp"

#include <cassert>

namespace pmt::parser::builder {
using namespace pmt::base;

auto Program::fetch_instruction(ProgramCounterType pc_) const -> Instruction {
 return _instructions[pc_];
}

auto Program::fetch_rule_info(ArgType rule_info_id_) const -> pmt::parser::rt::RuleInfo {
 return _rule_info_table[rule_info_id_];
}

auto Program::lit_seq_match_at(ArgType lit_seq_id_, size_t pos_, SymbolType sym_) const -> bool {
 assert(pos_ < get_lit_seq_length(lit_seq_id_));
 return _lit_seq_table[lit_seq_id_][pos_].contains(sym_);
}

auto Program::get_lit_seq_length(ArgType lit_seq_id_) const -> size_t {
 assert(lit_seq_id_ < _lit_seq_table.size());
 return _lit_seq_table[lit_seq_id_].size();
}

auto Program::get_entry_pc() const -> ProgramCounterType {
 return _entry_pc;
}

auto Program::get_max_pc() const -> ProgramCounterType {
 return _instructions.size();
}

void Program::set_entry_pc(ProgramCounterType pc_) {
 _entry_pc = pc_;
}

auto Program::push_instruction(Instruction instruction_) -> ProgramCounterType {
 _instructions.push_back(instruction_);
 return get_max_pc();
}

auto Program::add_rule_info(pmt::parser::rt::RuleInfo rule_info_) -> ArgType {
 auto const itr = std::ranges::find_if(_rule_info_table, [&](pmt::parser::rt::RuleInfo const& item_){return rule_info_ == item_;});

 if (itr != _rule_info_table.end()) {
  return std::distance(_rule_info_table.begin(), itr);
 }

 _rule_info_table.push_back(rule_info_);
 return _rule_info_table.size() - 1;
}

auto Program::add_lit_seq(LitSeqType lit_seq_) -> ArgType {
 auto const itr = std::ranges::find_if(_lit_seq_table, [&](LitSeqType const& item_){return lit_seq_ == item_;});

 if (itr != _lit_seq_table.end()) {
  return std::distance(_lit_seq_table.begin(), itr);
 }

 _lit_seq_table.push_back(std::move(lit_seq_));
 return _lit_seq_table.size() - 1;
}

}  // namespace pmt::parser::builder