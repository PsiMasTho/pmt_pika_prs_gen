#pragma once

#include "pmt/base/interval_set.hpp"
#include "pmt/parser/bytecode.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <vector>

namespace pmt::parser::builder {

class TypedProgram {
public:
 std::vector<Instruction> _instructions;
 std::unordered_map<ArgType, RuleInfo> _rule_map;
 std::unordered_map<ArgType, LabelInfo> _label_map;
 std::unordered_map<ArgType, LitSeqInfo> _lit_seq_map;
};
}  // namespace pmt::parser::builder