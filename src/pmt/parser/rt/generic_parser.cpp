#include "pmt/parser/rt/generic_parser.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parser/rt/parser_tables_base.hpp"
#include "pmt/parser/rt/util.hpp"
#include "pmt/parser/primitives.hpp"

namespace pmt::parser::rt {
 using namespace pmt::base;
 using namespace pmt::util::sm;

namespace {
auto add_child(GenericAst::UniqueHandle& root_, GenericAst::UniqueHandle child_, GenericAstPath const& path_) -> size_t {
 GenericAst* const parent = path_.resolve(*root_);

 assert(parent->get_tag() == GenericAst::Tag::Children);

 parent->give_child_at_back(std::move(child_));
 return parent->get_children_size() - 1;
}

}

auto GenericParser::parse(GenericLexer& lexer_, ParserTablesBase const& parser_tables_) -> GenericAst::UniqueHandle {
 _lexer = &lexer_;
 _parser_tables = &parser_tables_;
 _state_nr_cur = StateNrStart;
 _ast_root = GenericAst::construct(GenericAst::Tag::Children, GenericId::IdRoot);
 _conflict_count = _parser_tables->get_conflict_count();

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
    _state_nr_cur = _parser_tables->get_state_nr_next(_state_nr_cur, SymbolKindOpen, SymbolValueOpen);
   }
   break;
   case ParserTablesBase::StateTypeAccept: {
    size_t const accept_idx = _parser_tables->get_state_accept_index(_state_nr_cur);
    assert(accept_idx != AcceptIndexInvalid);
    if (accept_idx == _parser_tables->get_eoi_accept_index()) {
     _state_nr_cur = StateNrSink;
     break;
    }

    StackItem const stack_item = parse_stack_take();
    GenericAst* const parent = stack_item._ast_path.resolve(*_ast_root);
    parent->set_id(_parser_tables->get_accept_index_id(accept_idx));
    
    if (_parser_tables->get_accept_index_merge(accept_idx)) {
     parent->merge();
    }
    
    if (_parser_tables->get_accept_index_unpack(accept_idx)) {
     GenericAst* const grandparent = stack_item._ast_path.clone_pop().resolve(*_ast_root);
     grandparent->unpack(grandparent->get_children_size() - 1);
    }

    if (_parser_tables->get_accept_index_hide(accept_idx)) {
     GenericAst* const grandparent = stack_item._ast_path.clone_pop().resolve(*_ast_root);
     grandparent->take_child_at_back();
    }

    _state_nr_cur = _parser_tables->get_state_nr_next(stack_item._state_nr, SymbolKindClose, accept_idx);
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

    if (get_bit(_parser_tables->get_state_hidden_terminal_transitions(_state_nr_cur), lexed._accepted)) {
     _state_nr_cur = _parser_tables->get_state_nr_next(_state_nr_cur, SymbolKindHiddenTerminal, lexed._accepted);
    } else {
     _state_nr_cur = _parser_tables->get_state_nr_next(_state_nr_cur, SymbolKindTerminal, lexed._accepted);
     add_child(_ast_root, lexed._token.to_ast(), ast_path_cur);
    }
   }
   break;
   case ParserTablesBase::StateTypeConflict: {
    Bitset::ChunkSpanConst const conflict_transitions = _parser_tables->get_state_conflict_transitions(_state_nr_cur);
    StateNrType state_nr_conflict_cur = StateNrStart;

    size_t const consume_from_queue = _lex_queue.size();
    size_t consumed_from_queue = 0;

    while (true) {
     if (state_nr_conflict_cur == StateNrSink) {
      break;
     }

     Bitset::ChunkSpanConst const conflict_accepts = _parser_tables->get_lookahead_state_accepts(state_nr_conflict_cur);
     bitwise_and(_conflict_accepts_valid, conflict_accepts, conflict_transitions);
     size_t const countl = find_first_set_bit(_conflict_accepts_valid);

     if (countl < _conflict_count) {
      _state_nr_cur = _parser_tables->get_state_nr_next(_state_nr_cur, SymbolKindConflict, countl);
      break;
     }

     size_t const accepted = [&]() {
      if (consumed_from_queue < consume_from_queue) {
       return _lex_queue[consumed_from_queue++]._accepted;
      } else {
       _lex_queue.push_back(_lexer->lex(_parser_tables->get_lookahead_state_terminal_transitions(state_nr_conflict_cur)));
       return _lex_queue.back()._accepted;
      }
     }();
     state_nr_conflict_cur = _parser_tables->get_lookahead_state_nr_next(state_nr_conflict_cur, accepted);
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

auto GenericParser::parse_stack_take() -> StackItem {
 assert(!_parse_stack.empty());
 StackItem stack_item = _parse_stack.back();
 _parse_stack.pop_back();
 return stack_item;
}

}