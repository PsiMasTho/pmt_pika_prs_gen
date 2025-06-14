#include "pmt/parser/rt/generic_parser.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/bitset.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/parser/rt/parser_tables_base.hpp"
#include "pmt/parser/rt/util.hpp"
#include "pmt/util/sm/primitives.hpp"

#include <cassert>
#include <deque>
#include <vector>

namespace pmt::parser::rt {
using namespace pmt::base;
using namespace pmt::util::sm;

namespace {

class StackItem {
 public:
  StateNrType _state_nr;
  GenericAst* _ast;
};

class Locals {
 public:
  std::vector<pmt::base::Bitset::ChunkType> _conflict_accepts_valid;
  std::vector<StackItem> _parse_stack;
  std::deque<GenericLexer::LexReturn> _lex_queue;
  GenericLexer* _lexer = nullptr;
  ParserTablesBase const* _parser_tables = nullptr;
  pmt::util::sm::StateNrType _state_nr_cur = pmt::util::sm::StateNrStart;
  GenericAst::UniqueHandle _ast_root = nullptr;
  size_t _conflict_count = 0;
};

auto add_child(GenericAst& parent_, GenericAst::UniqueHandle child_) -> GenericAst* {
  assert(parent_.get_tag() == GenericAst::Tag::Children);

  parent_.give_child_at_back(std::move(child_));
  return parent_.get_child_at_back();
}

auto parse_stack_take(Locals& locals_) -> StackItem {
  assert(!locals_._parse_stack.empty());
  StackItem stack_item = locals_._parse_stack.back();
  locals_._parse_stack.pop_back();
  return stack_item;
}

}  // namespace

auto GenericParser::parse(Args args_) -> GenericAst::UniqueHandle {
  Locals locals;

  locals._ast_root = GenericAst::construct(GenericAst::Tag::Children, GenericId::IdRoot);
  locals._conflict_count = args_._parser_tables.get_conflict_count();

  locals._conflict_accepts_valid.resize(Bitset::get_required_chunk_count(args_._parser_tables.get_conflict_count()), Bitset::ALL_SET_MASKS[false]);

  while (true) {
    if (locals._state_nr_cur == StateNrSink) {
      break;
    }

    GenericAst& ast_cur = locals._parse_stack.empty() ? *locals._ast_root.get() : *locals._parse_stack.back()._ast;

    switch (args_._parser_tables.get_state_kind(locals._state_nr_cur)) {
      case ParserTablesBase::StateTypeOpen: {
        StackItem stack_item{._state_nr = locals._state_nr_cur, ._ast = add_child(ast_cur, GenericAst::construct(GenericAst::Tag::Children))};
        locals._parse_stack.push_back(stack_item);
        locals._state_nr_cur = args_._parser_tables.get_state_nr_next(locals._state_nr_cur, SymbolKindOpen, SymbolValueOpen);
      } break;
      case ParserTablesBase::StateTypeAccept: {
        size_t const accept_idx = args_._parser_tables.get_state_accept_index(locals._state_nr_cur);
        assert(accept_idx != AcceptIndexInvalid);
        if (accept_idx == args_._parser_tables.get_eoi_accept_index()) {
          locals._state_nr_cur = StateNrSink;
          break;
        }

        StackItem const stack_item = parse_stack_take(locals);
        ast_cur.set_id(args_._parser_tables.get_accept_index_id(accept_idx));

        if (args_._parser_tables.get_accept_index_merge(accept_idx)) {
          ast_cur.merge();
        }

        GenericAst& grandparent = locals._parse_stack.empty() ? *locals._ast_root.get() : *locals._parse_stack.back()._ast;
        if (args_._parser_tables.get_accept_index_unpack(accept_idx)) {
          grandparent.unpack(grandparent.get_children_size() - 1);
        }

        if (args_._parser_tables.get_accept_index_hide(accept_idx)) {
          grandparent.take_child_at_back();
        }

        locals._state_nr_cur = args_._parser_tables.get_state_nr_next(stack_item._state_nr, SymbolKindClose, accept_idx);
      } break;
      case ParserTablesBase::StateTypeTerminal: {
        GenericLexer::LexReturn lexed;
        if (!locals._lex_queue.empty()) {
          lexed = locals._lex_queue.front();
          locals._lex_queue.pop_front();
        } else {
          lexed = args_._lexer.lex(args_._parser_tables.get_state_terminal_transitions(locals._state_nr_cur));
        }

        if (get_bit(args_._parser_tables.get_state_hidden_terminal_transitions(locals._state_nr_cur), lexed._accepted)) {
          locals._state_nr_cur = args_._parser_tables.get_state_nr_next(locals._state_nr_cur, SymbolKindHiddenTerminal, lexed._accepted);
        } else {
          locals._state_nr_cur = args_._parser_tables.get_state_nr_next(locals._state_nr_cur, SymbolKindTerminal, lexed._accepted);
          add_child(ast_cur, lexed._token.to_ast());
        }
      } break;
      case ParserTablesBase::StateTypeConflict: {
        Bitset::ChunkSpanConst const conflict_transitions = args_._parser_tables.get_state_conflict_transitions(locals._state_nr_cur);
        StateNrType state_nr_conflict_cur = StateNrStart;

        size_t const consume_from_queue = locals._lex_queue.size();
        size_t consumed_from_queue = 0;

        while (true) {
          if (state_nr_conflict_cur == StateNrSink) {
            break;
          }

          Bitset::ChunkSpanConst const conflict_accepts = args_._parser_tables.get_lookahead_state_accepts(state_nr_conflict_cur);
          bitwise_and(locals._conflict_accepts_valid, conflict_accepts, conflict_transitions);
          size_t const countl = find_first_set_bit(locals._conflict_accepts_valid);

          if (countl < locals._conflict_count) {
            locals._state_nr_cur = args_._parser_tables.get_state_nr_next(locals._state_nr_cur, SymbolKindConflict, countl);
            break;
          }

          size_t const accepted = [&]() {
            if (consumed_from_queue < consume_from_queue) {
              return locals._lex_queue[consumed_from_queue++]._accepted;
            } else {
              try {
                locals._lex_queue.push_back(args_._lexer.lex(args_._parser_tables.get_lookahead_state_terminal_transitions(state_nr_conflict_cur)));
              } catch (...) {
                throw std::runtime_error("No valid lexed token found for conflict resolution.");  // TODO: we should report what token(s) we expected
              }
              return locals._lex_queue.back()._accepted;
            }
          }();
          state_nr_conflict_cur = args_._parser_tables.get_lookahead_state_nr_next(state_nr_conflict_cur, accepted);
        }
      } break;
      default: {
        pmt::unreachable();
      }
    }
  }

  return std::move(locals._ast_root);
}

}  // namespace pmt::parser::rt