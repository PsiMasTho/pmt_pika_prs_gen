#pragma once

#include "pmt/parser/bytecode.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/primitives.hpp"

namespace pmt::parser::rt {

class RuleInfo {
public:
 GenericId::IdType _rule_id;
};

class ProgramBase {
public:
 // -$ Functions $-
 // --$ Other $--
 [[nodiscard]] virtual auto fetch_instruction(ProgramCounterType pc_) -> Instruction;
 [[nodiscard]] virtual auto fetch_rule_info(ArgType rule_id_) -> RuleInfo;

 [[nodiscard]] virtual auto lit_seq_match_at(ArgType lit_seq_id_, size_t pos_, SymbolType sym_) -> bool;
 [[nodiscard]] virtual auto get_lit_seq_length(ArgType lit_seq_id_) -> size_t;

 [[nodiscard]] virtual auto get_entry_pc(ArgType label_id_) -> ProgramCounterType;
 [[nodiscard]] virtual auto get_max_pc(ArgType label_id_) -> ProgramCounterType;
};

}  // namespace pmt::parser::rt
