#include "pmt/rt/pika_parser.hpp"

#include "pmt/rt/clause_matcher.hpp"
#include "pmt/rt/clause_queue.hpp"
#include "pmt/rt/memo_table.hpp"
#include "pmt/rt/pika_program_base.hpp"
#include "pmt/rt/rule_parameters_base.hpp"
#include "pmt/rt/state_machine_tables_base.hpp"
#include "pmt/unreachable.hpp"

#include <functional>

namespace pmt::rt {
namespace {

auto check_parse_success(MemoTable const& memo_table_) -> bool {
 ClauseBase const& start_clause = memo_table_.get_pika_program().fetch_clause(0);
 MemoTable::Key start_key{._clause = &start_clause, ._position = 0};
 MemoTable::IndexType const start_match_index = memo_table_.find(start_key);
 return start_match_index != MemoTable::MemoIndexMatchNotFound && memo_table_.get_match_length_by_index(start_match_index) == memo_table_.get_input().size();
}

void scan_terminals(StateMachineTablesBase const& terminal_state_machine_tables_, size_t cursor_, MemoTable& memo_table_, ClauseQueue& clause_queue_) {
 size_t const cursor_start = cursor_;
 StateNrType state_nr_cur = StateNrStart;

 for (; cursor_ < memo_table_.get_input().size() + 1 && state_nr_cur != StateNrInvalid; ++cursor_) {
  if (size_t const accept_count = terminal_state_machine_tables_.get_state_final_id_count(state_nr_cur); accept_count > 0) {
   for (size_t i = 0; i < accept_count; ++i) {
    FinalIdType const accept_ = terminal_state_machine_tables_.get_state_final_id(state_nr_cur, i);
    MemoTable::Match match{
     ._key = {._clause = &memo_table_.get_pika_program().fetch_clause(accept_), ._position = cursor_start},
     ._length = cursor_ - cursor_start,
     ._matching_subclauses = {},
    };

    memo_table_.insert(match._key, match, clause_queue_);
   }
  }

  if (cursor_ == memo_table_.get_input().size()) {
   break;
  }

  state_nr_cur = terminal_state_machine_tables_.get_state_nr_next(state_nr_cur, memo_table_.get_input()[cursor_]);
 }
}

}  // namespace

auto PikaParser::populate_memo_table(PikaProgramBase const& pika_program_, std::string_view input_) -> MemoTable {
 MemoTable memo_table(pika_program_, input_);
 ClauseQueue clause_queue;

 for (size_t cursor = input_.size(); cursor-- > 0;) {
  scan_terminals(pika_program_.get_terminal_state_machine_tables(), cursor, memo_table, clause_queue);

  while (!clause_queue.empty()) {
   ClauseQueueItem item = clause_queue.top();
   clause_queue.pop();

   MemoTable::Key key{._clause = item._clause, ._position = cursor};
   std::optional<MemoTable::Match> match = ClauseMatcher::match(memo_table, key, input_);
   memo_table.insert(key, std::move(match), clause_queue);
  }
 }

 return memo_table;
}

auto PikaParser::memo_table_to_ast(MemoTable const& memo_table_) -> Ast::UniqueHandle {
 if (!check_parse_success(memo_table_)) {
  return nullptr;
 }

 Ast::UniqueHandle ast_root = Ast::construct(Ast::Tag::Parent, AstId::IdRoot);

 auto const process_and_add_child = [&](Ast& parent_, Ast::UniqueHandle child_, MemoTable::Match const& child_match_) {
  if (!child_) {
   return;
  }

  switch (child_match_._key._clause->get_tag()) {
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::OneOrMore:
    parent_.give_child_at_back(std::move(child_));
    parent_.unpack(parent_.get_children_size() - 1);
    break;
   case ClauseBase::Tag::CharsetLiteral:
    parent_.give_child_at_back(std::move(child_));
    break;
   case ClauseBase::Tag::Identifier: {
    // apply rule parameters
    RuleParametersBase const& rule_parameters = memo_table_.get_pika_program().fetch_rule_parameters(child_match_._key._clause->get_rule_id());
    if (rule_parameters.get_hide()) {
     break;
    }
    if (rule_parameters.get_merge()) {
     child_->merge();
    }
    child_->set_id(rule_parameters.get_id_value());
    parent_.give_child_at_back(std::move(child_));
    if (rule_parameters.get_unpack()) {
     parent_.unpack(parent_.get_children_size() - 1);
    }
   } break;
   case ClauseBase::Tag::NegativeLookahead:
    break;
   case ClauseBase::Tag::Epsilon:
    break;
  }
 };

 std::function<Ast::UniqueHandle(MemoTable::Match const&)> recursive_worker = [&](MemoTable::Match const& match_) -> Ast::UniqueHandle {
  Ast::UniqueHandle ast_node;

  switch (match_._key._clause->get_tag()) {
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::Identifier:
    ast_node = Ast::construct(Ast::Tag::Parent, AstId::IdUninitialized);
    for (MemoTable::IndexType const child_match_index : match_._matching_subclauses) {
     if (child_match_index == MemoTable::MemoIndexMatchNotFound || child_match_index == MemoTable::MemoIndexMatchZeroLength) {
      continue;
     }
     MemoTable::Match const& child_match = memo_table_.get_match_by_index(child_match_index);
     process_and_add_child(*ast_node, recursive_worker(child_match), child_match);
    }
    break;
   case ClauseBase::Tag::CharsetLiteral:
    ast_node = Ast::construct(Ast::Tag::String, AstId::IdDefault);
    ast_node->set_string(memo_table_.get_input().substr(match_._key._position, match_._length));
    break;
   case ClauseBase::Tag::NegativeLookahead:
   case ClauseBase::Tag::Epsilon:
    // No AST node
    break;
   default:
    pmt::unreachable();
  }

  return ast_node;
 };

 if (MemoTable::IndexType const start_match_index = memo_table_.find(MemoTable::Key{._clause = &memo_table_.get_pika_program().fetch_clause(0), ._position = 0}); start_match_index != MemoTable::MemoIndexMatchNotFound && start_match_index != MemoTable::MemoIndexMatchZeroLength) {
  MemoTable::Match const& start_match = memo_table_.get_match_by_index(start_match_index);
  process_and_add_child(*ast_root, recursive_worker(start_match), start_match);
 }

 return ast_root;
}

}  // namespace pmt::rt