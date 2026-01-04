#include "pmt/parser/rt/pika_parser.hpp"

#include "pmt/parser/rt/clause_matcher.hpp"
#include "pmt/parser/rt/clause_queue.hpp"
#include "pmt/parser/rt/memo_table.hpp"
#include "pmt/parser/rt/pika_program_base.hpp"
#include "pmt/parser/rt/state_machine_tables_base.hpp"
#include "pmt/parser/rt/util.hpp"

#include <iostream>

namespace pmt::parser::rt {
using namespace pmt::sm;
using namespace pmt::base;
namespace {

void debug_print_memo_table(MemoTable const& memo_table_) {
 ClauseBase const& start_clause = memo_table_.get_pika_program().fetch_clause(0);
 MemoTable::Key start_key{._clause = &start_clause, ._position = 0};
 MemoTable::IndexType const start_match_index = memo_table_.find(start_key);
 if (start_match_index != MemoTable::MemoIndexMatchNotFound && memo_table_.get_match_length_by_index(start_match_index) == memo_table_.get_input().size()) {
  std::cout << "!PARSING SUCCESS!\n";
 } else {
  std::cout << "!PARSING FAILED!.\n";
 }

 for (size_t i = 0; i < memo_table_.get_match_count(); ++i) {
  MemoTable::Match const& match = memo_table_.get_match_by_index(i);
  std::cout << "Match " << i << ": Clause ID " << match._key._clause->get_id() << ", Tag " << ClauseBase::tag_to_string(match._key._clause->get_tag());

  if (match._key._clause->get_tag() == ClauseBase::Tag::Identifier) {
   std::cout << " (" << memo_table_.get_pika_program().fetch_non_terminal(match._key._clause->get_non_terminal_id())._display_name << ") ";
  }

  std::cout << ", Position " << match._key._position << ", Length " << match._length << ", Text: '" << memo_table_.get_input().substr(match._key._position, match._length) << "'\n";
 }
}

void scan_terminals(StateMachineTablesBase const& terminal_state_machine_tables_, size_t cursor_, MemoTable& memo_table_, ClauseQueue& clause_queue_) {
 size_t const cursor_start = cursor_;
 size_t const accepts_count = terminal_state_machine_tables_.get_accept_count();

 StateNrType state_nr_cur = StateNrStart;

 std::vector<Bitset::ChunkType> accepts(accepts_count, Bitset::ALL_SET_MASKS[false]);

 for (; cursor_ < memo_table_.get_input().size() + 1 && state_nr_cur != StateNrInvalid; ++cursor_) {
  // Check for accepting state
  if (auto const& accepts_span = terminal_state_machine_tables_.get_state_accepts(state_nr_cur); find_first_set_bit(accepts_span) < accepts_count) {
   std::copy(accepts_span.begin(), accepts_span.end(), accepts.begin());
   while (true) {
    size_t const first_set_bit = find_first_set_bit(accepts);
    if (first_set_bit >= accepts_count) {
     break;
    }

    MemoTable::Match match{
     ._key = {._clause = &memo_table_.get_pika_program().fetch_clause(first_set_bit), ._position = cursor_start},
     ._length = cursor_ - cursor_start,
     ._matching_subclauses = {},
    };

    memo_table_.insert(match._key, match, clause_queue_);
    clear_bit(accepts, first_set_bit);
   }
  }

  state_nr_cur = terminal_state_machine_tables_.get_state_nr_next(state_nr_cur, memo_table_.get_input()[cursor_]);
 }
}

auto populate_memo_table(PikaProgramBase const& pika_program_, std::string_view input_) -> MemoTable {
 MemoTable memo_table(pika_program_, input_);
 ClauseQueue clause_queue;

 for (size_t cursor = input_.size(); cursor-- > 0;) {
  scan_terminals(pika_program_.get_literal_state_machine_tables(), cursor, memo_table, clause_queue);

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

}  // namespace

auto PikaParser::parse(PikaProgramBase const& pika_program_, std::string_view input_) -> GenericAst::UniqueHandle {
 MemoTable const memo_table = populate_memo_table(pika_program_, input_);

 debug_print_memo_table(memo_table);

 // Build AST from memo table
 // ...
 return nullptr;
}

}  // namespace pmt::parser::rt