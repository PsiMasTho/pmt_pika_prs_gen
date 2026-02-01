#include "pmt/rt/pika_parser.hpp"

#include "pmt/rt/clause_matcher.hpp"
#include "pmt/rt/clause_queue.hpp"
#include "pmt/rt/memo_table.hpp"
#include "pmt/rt/pika_program_base.hpp"
#include "pmt/rt/rule_parameters_base.hpp"
#include "pmt/rt/state_machine_tables_base.hpp"
#include "pmt/unreachable.hpp"

namespace pmt::rt {
namespace {

auto check_parse_success(MemoTable const& memo_table_, std::string_view input_, PikaProgramBase const& pika_program_) -> bool {
 ClauseBase const& start_clause = pika_program_.fetch_clause(0);
 MemoTable::Key start_key{._clause = &start_clause, ._position = 0};
 MemoTable::IndexType const start_match_index = memo_table_.find(start_key, input_, pika_program_);
 return start_match_index != MemoTable::MemoIndexMatchNotFound && memo_table_.get_match_length_by_index(start_match_index) == input_.size();
}

void scan_terminals(StateMachineTablesBase const& terminal_state_machine_tables_, size_t cursor_, MemoTable& memo_table_, ClauseQueue& clause_queue_, std::string_view input_, PikaProgramBase const& pika_program_) {
 size_t const cursor_start = cursor_;
 StateNrType state_nr_cur = StateNrStart;

 for (; cursor_ < input_.size() + 1 && state_nr_cur != StateNrInvalid; ++cursor_) {
  if (size_t const accept_count = terminal_state_machine_tables_.get_state_final_id_count(state_nr_cur); accept_count > 0) {
   for (size_t i = 0; i < accept_count; ++i) {
    FinalIdType const accept_ = terminal_state_machine_tables_.get_state_final_id(state_nr_cur, i);
    MemoTable::Match match{
     ._key_index = MemoTable::MemoIndexKeyUninitialized,
     ._length = cursor_ - cursor_start,
     ._matching_subclauses = {},
    };

    memo_table_.insert(MemoTable::Key{._clause = &pika_program_.fetch_clause(accept_), ._position = cursor_start}, match, clause_queue_, pika_program_);
   }
  }

  if (cursor_ == input_.size()) {
   break;
  }

  state_nr_cur = terminal_state_machine_tables_.get_state_nr_next(state_nr_cur, input_[cursor_]);
 }
}

void process_and_add_child(Ast& parent_, Ast::UniqueHandle child_, MemoTable::Match const& child_match_, MemoTable const& memo_table_, PikaProgramBase const& pika_program_) {
 if (!child_) {
  return;
 }

 MemoTable::Key const& child_key = memo_table_.get_key_by_index(child_match_._key_index);
 switch (child_key._clause->get_tag()) {
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
   RuleParametersBase const& rule_parameters = pika_program_.fetch_rule_parameters(child_key._clause->get_rule_id());
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
}

auto build_ast(MemoTable::Match const& root_match_, MemoTable const& memo_table_, std::string_view input_, PikaProgramBase const& pika_program_) -> Ast::UniqueHandle {
 MemoTable::Key const& root_key = memo_table_.get_key_by_index(root_match_._key_index);
 ClauseBase::Tag const root_tag = root_key._clause->get_tag();
 if (root_tag == ClauseBase::Tag::NegativeLookahead || root_tag == ClauseBase::Tag::Epsilon) {
  return nullptr;
 }
 if (root_tag == ClauseBase::Tag::CharsetLiteral) {
  Ast::UniqueHandle node = Ast::construct(Ast::Tag::String, AstId::IdDefault);
  node->set_string(input_.substr(root_key._position, root_match_._length));
  return node;
 }
 assert(root_tag == ClauseBase::Tag::Sequence || root_tag == ClauseBase::Tag::Choice || root_tag == ClauseBase::Tag::OneOrMore || root_tag == ClauseBase::Tag::Identifier);

 struct Frame {
  MemoTable::Match const* _match;
  size_t _next_child_idx;
  Ast::UniqueHandle _ast_node;
 };

 std::vector<Frame> stack;
 stack.push_back(Frame{&root_match_, 0, Ast::construct(Ast::Tag::Parent, AstId::IdUninitialized)});

 Ast::UniqueHandle result;

 while (!stack.empty()) {
  Frame& frame = stack.back();
  MemoTable::Match const& match = *frame._match;

  if (frame._next_child_idx >= match._matching_subclauses.size()) {
   Ast::UniqueHandle completed = std::move(frame._ast_node);
   stack.pop_back();
   if (stack.empty()) {
    result = std::move(completed);
    break;
   }
   Frame& parent = stack.back();
   process_and_add_child(*parent._ast_node, std::move(completed), match, memo_table_, pika_program_);
   continue;
  }

  MemoTable::IndexType const child_match_index = match._matching_subclauses[frame._next_child_idx++];
  if (child_match_index == MemoTable::MemoIndexMatchNotFound || child_match_index == MemoTable::MemoIndexMatchZeroLength) {
   continue;
  }

  MemoTable::Match const& child_match = memo_table_.get_match_by_index(child_match_index);
  MemoTable::Key const& child_key = memo_table_.get_key_by_index(child_match._key_index);
  switch (child_key._clause->get_tag()) {
   case ClauseBase::Tag::Sequence:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::Identifier:
    stack.push_back(Frame{&child_match, 0, Ast::construct(Ast::Tag::Parent, AstId::IdUninitialized)});
    break;
   case ClauseBase::Tag::CharsetLiteral: {
    Ast::UniqueHandle node = Ast::construct(Ast::Tag::String, AstId::IdDefault);
    node->set_string(input_.substr(child_key._position, child_match._length));
    process_and_add_child(*frame._ast_node, std::move(node), child_match, memo_table_, pika_program_);
   } break;
   case ClauseBase::Tag::NegativeLookahead:
   case ClauseBase::Tag::Epsilon:
    break;
   default:
    pmt::unreachable();
  }
 }

 return result;
}

}  // namespace

auto PikaParser::populate_memo_table(std::string_view input_, PikaProgramBase const& pika_program_) -> MemoTable {
 MemoTable memo_table;
 ClauseQueue clause_queue;

 for (size_t cursor = input_.size(); cursor-- > 0;) {
  scan_terminals(pika_program_.get_terminal_state_machine_tables(), cursor, memo_table, clause_queue, input_, pika_program_);

  while (!clause_queue.empty()) {
   ClauseQueueItem item = clause_queue.top();
   clause_queue.pop();

   MemoTable::Key key{._clause = item._clause, ._position = cursor};
   std::optional<MemoTable::Match> match = ClauseMatcher::match(memo_table, key, input_, pika_program_);
   memo_table.insert(key, std::move(match), clause_queue, pika_program_);
  }
 }

 return memo_table;
}

auto PikaParser::memo_table_to_ast(MemoTable const& memo_table_, std::string_view input_, PikaProgramBase const& pika_program_) -> Ast::UniqueHandle {
 if (!check_parse_success(memo_table_, input_, pika_program_)) {
  return nullptr;
 }

 Ast::UniqueHandle ast_root = Ast::construct(Ast::Tag::Parent, AstId::IdRoot);

 if (MemoTable::IndexType const start_match_index = memo_table_.find(MemoTable::Key{._clause = &pika_program_.fetch_clause(0), ._position = 0}, input_, pika_program_); start_match_index != MemoTable::MemoIndexMatchNotFound && start_match_index != MemoTable::MemoIndexMatchZeroLength) {
  MemoTable::Match const& start_match = memo_table_.get_match_by_index(start_match_index);
  process_and_add_child(*ast_root, build_ast(start_match, memo_table_, input_, pika_program_), start_match, memo_table_, pika_program_);
 }

 return ast_root;
}

}  // namespace pmt::rt
