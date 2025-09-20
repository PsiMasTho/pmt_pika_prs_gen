#include "pmt/parser/builder/program.hpp"

#include "pmt/base/hash.hpp"

#include <cassert>

namespace pmt::parser::builder {
using namespace pmt::base;

Program::LitSeqTypeIndirectHasher::LitSeqTypeIndirectHasher(std::vector<LitSeqType> const& table_)
 : _table(table_) {
}

auto Program::LitSeqTypeIndirectHasher::operator()(ArgType lit_seq_id_) const -> size_t {
 assert(lit_seq_id_ < _table.size());
 return this->operator()(_table[lit_seq_id_]);
}

auto Program::LitSeqTypeIndirectHasher::operator()(LitSeqType const& lit_seq_) const -> size_t {
 size_t seed = pmt::base::Hash::Phi64;

 for (auto const& element : lit_seq_) {
  pmt::base::Hash::combine(element, seed);
 }

 return seed;
}

Program::LitSeqTypeIndirectEq::LitSeqTypeIndirectEq(std::vector<LitSeqType> const& table_)
 : _table(table_) {
}

Program::Program()
 : _lit_seq_table{}
 , _lit_seq_table_indirect(0, LitSeqTypeIndirectHasher(_lit_seq_table), LitSeqTypeIndirectEq(_lit_seq_table))
 , _instructions{}
 , _rule_info_table{}
 , _entry_pc(0) {
}

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
}

auto Program::add_lit_seq(LitSeqType lit_seq_) -> ArgType {
}

}  // namespace pmt::parser::builder