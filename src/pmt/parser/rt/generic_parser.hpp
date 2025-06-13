#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/generic_ast_path.hpp"
#include "pmt/parser/rt/generic_lexer.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <deque>
#include <vector>

PMT_FW_DECL_NS_CLASS(pmt::parser::rt, ParserTablesBase);

namespace pmt::parser::rt {

class GenericParser {
  // -$ Types / Constants $-
  struct StackItem {
    pmt::util::sm::StateNrType _state_nr;
    GenericAstPath _ast_path;
  };

  // -$ Data $-
  std::vector<pmt::base::Bitset::ChunkType> _conflict_accepts_valid;
  std::vector<StackItem> _parse_stack;
  std::deque<GenericLexer::LexReturn> _lex_queue;
  GenericLexer* _lexer = nullptr;
  ParserTablesBase const* _parser_tables = nullptr;
  pmt::util::sm::StateNrType _state_nr_cur = pmt::util::sm::StateNrStart;
  GenericAst::UniqueHandle _ast_root = nullptr;
  size_t _conflict_count = 0;

 public:
  auto parse(GenericLexer& lexer_, ParserTablesBase const& parser_tables_) -> GenericAst::UniqueHandle;

 private:
  auto parse_stack_take() -> StackItem;
};

}  // namespace pmt::parser::rt