#include "pmt/util/smrt/generic_parser.hpp"

#include "pmt/asserts.hpp"
#include "pmt/util/smrt/parser_tables_base.hpp"

namespace pmt::util::smrt {
 using namespace pmt::base;

namespace {
auto find_first_set_bit(Bitset::ChunkSpanConst const& bitset_) -> size_t {
 size_t total = 0;
 for (Bitset::ChunkType chunk : bitset_) {
   size_t const incr = std::countr_zero(chunk);
   total += incr;
   if (incr != Bitset::ChunkBit) {
     break;
   }
 }

 return total;
}

auto add_child(GenericAst::UniqueHandle& root_, GenericAst::UniqueHandle child_, GenericAstPath const& path_) -> size_t {
 GenericAst* const parent = path_.resolve(*root_);

 assert(parent->get_tag() == GenericAst::Tag::Children);

 parent->give_child_at_back(std::move(child_));
 return parent->get_children_size() - 1;
}

void bitwise_and(Bitset::ChunkSpan dest_, Bitset::ChunkSpanConst lhs_ , Bitset::ChunkSpanConst rhs_) {
 std::fill(dest_.begin(), dest_.end(), Bitset::ChunkType(0));

 Bitset::ChunkSpanConst const* smaller = lhs_.size() < rhs_.size() ? &lhs_ : &rhs_;
 Bitset::ChunkSpanConst const* larger = lhs_.size() < rhs_.size() ? &rhs_ : &lhs_;

 for (size_t i = 0; i < smaller->size(); ++i) {
   dest_[i] = (*smaller)[i] & (*larger)[i];
 }
}

}

auto GenericParser::parse(GenericLexer& lexer_, ParserTablesBase const& parser_tables_) -> GenericAst::UniqueHandle {
 static SymbolType const symbol_open = (SymbolKindOpen << SymbolValueBitWidth) | SymbolValueOpen;

 _lexer = &lexer_;
 _parser_tables = &parser_tables_;
 _state_nr_cur = StateNrStart;
 _ast_root = GenericAst::construct(GenericAst::Tag::Children);

 _conflict_accepts_valid.resize(Bitset::get_required_chunk_count(_parser_tables->get_conflict_count()), Bitset::ALL_SET_MASKS[false]);

 while (true) {
  if (_state_nr_cur == StateNrSink) {
   break;
  }

  GenericAstPath ast_path_cur = _parse_stack.empty() ? GenericAstPath{} : _parse_stack.back()._ast_path;

  switch (_parser_tables->get_state_kind(_state_nr_cur)) {
   case ParserTablesBase::StateTypeOpen: {
    StackItem stack_item{
     ._state_nr = _state_nr_cur,
     ._ast_path = ast_path_cur.inplace_push(add_child(_ast_root, GenericAst::construct(GenericAst::Tag::Children), ast_path_cur))
    };
    _parse_stack.push_back(stack_item);
    _state_nr_cur = _parser_tables->get_state_nr_next(_state_nr_cur, symbol_open);
   }
   break;
   case ParserTablesBase::StateTypeAccept: {
    size_t const accept_idx = _parser_tables->get_state_accept_index(_state_nr_cur);
    assert(accept_idx != AcceptIndexInvalid);
    if (accept_idx == _lexer->get_eoi_accept_index() || _parse_stack.empty()) {
     _state_nr_cur = StateNrSink;
     break;
    }

    StackItem const stack_item = _parse_stack.back();
    _parse_stack.pop_back();
    GenericAst* const parent = stack_item._ast_path.resolve(*_ast_root);
    parent->set_id(_parser_tables->get_accept_index_id(accept_idx));

    _state_nr_cur = _parser_tables->get_state_nr_next(stack_item._state_nr, SymbolType((SymbolKindClose << SymbolValueBitWidth) | accept_idx));
   }
   break;
   case ParserTablesBase::StateTypeTerminal: {
    GenericLexer::LexReturn lexed;
    if (!_lex_queue.empty()) {
     lexed = _lex_queue.front();
     _lex_queue.pop_front();
    } else {
     lexed = _lexer->lex(_parser_tables->get_state_terminal_transitions(_state_nr_cur));
    }
    _state_nr_cur = _parser_tables->get_state_nr_next(_state_nr_cur, SymbolType((SymbolKindTerminal << SymbolValueBitWidth) | lexed._accepted));
    add_child(_ast_root, lexed._token.to_ast(), ast_path_cur);
   }
   break;
   case ParserTablesBase::StateTypeConflict: {
    Bitset::ChunkSpanConst const conflict_transitions = _parser_tables->get_state_conflict_transitions(_state_nr_cur);
    StateNrType state_nr_conflict_cur = StateNrStart;

    size_t consume_from_queue = _lex_queue.size();

    while (true) {
     if (state_nr_conflict_cur == StateNrSink) {
      break;
     }

     Bitset::ChunkSpanConst const conflict_accepts = _parser_tables->get_lookahead_state_accepts(state_nr_conflict_cur);
     bitwise_and(_conflict_accepts_valid, conflict_accepts, conflict_transitions);
     size_t const countl = find_first_set_bit(_conflict_accepts_valid);

     if (countl < _parser_tables->get_conflict_count()) {
      _state_nr_cur = _parser_tables->get_state_nr_next(_state_nr_cur, SymbolType((SymbolKindConflict << SymbolValueBitWidth) | countl));
      break;
     }

     GenericLexer::LexReturn lexed;
     if (consume_from_queue != 0) {
      lexed = _lex_queue.front();
      _lex_queue.pop_front();
      --consume_from_queue;
     } else {
      lexed = _lexer->lex(_parser_tables->get_lookahead_state_terminal_transitions(state_nr_conflict_cur));
      _lex_queue.push_back(lexed);
     }
     state_nr_conflict_cur = _parser_tables->get_lookahead_state_nr_next(state_nr_conflict_cur, SymbolType((SymbolKindTerminal << SymbolValueBitWidth) | lexed._accepted));
    }
   }
   break;
   default:{
    pmt::unreachable();
   }
  }
 }

 return std::move(_ast_root);
}


}