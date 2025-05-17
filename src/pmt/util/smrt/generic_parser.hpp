#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/base/bitset.hpp"
#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/generic_ast_path.hpp"
#include "pmt/util/smrt/generic_lexer.hpp"
#include "pmt/util/smrt/state_machine_primitives.hpp"

#include <deque>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, ParserTablesBase);

namespace pmt::util::smrt {

class GenericParser {
 // -$ Types / Constants $-
 struct StackItem {
  StateNrType _state_nr;
  GenericAstPath _ast_path;
 };

 // -$ Data $-
 std::vector<pmt::base::Bitset::ChunkType> _conflict_accepts_valid;
 std::vector<StackItem> _parse_stack;
 std::deque<GenericLexer::LexReturn> _lex_queue;
 GenericLexer* _lexer = nullptr;
 ParserTablesBase const* _parser_tables = nullptr;
 StateNrType _state_nr_cur = StateNrStart;
 GenericAst::UniqueHandle _ast_root = nullptr;

 public:
 auto parse(GenericLexer& lexer_, ParserTablesBase const& parser_tables_) -> GenericAst::UniqueHandle;
};

}