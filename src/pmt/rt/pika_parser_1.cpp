#include "pmt/rt/pika_parser.hpp"

#include "pmt/rt/clause_matcher.hpp"
#include "pmt/rt/clause_queue.hpp"
#include "pmt/rt/memo_table.hpp"
#include "pmt/rt/pika_tables_base.hpp"
#include "pmt/rt/state_machine_tables_base.hpp"

namespace pmt::rt {
namespace {

void scan_terminals(StateMachineTablesBase const& terminal_state_machine_tables_, size_t cursor_, MemoTable& memo_table_, ClauseQueue& clause_queue_, std::string_view input_, PikaTablesBase const& pika_tables_) {
 size_t const cursor_start = cursor_;
 StateNrType state_nr_cur = StateNrStart;

 for (; cursor_ < input_.size() + 1 && state_nr_cur != StateNrInvalid; ++cursor_) {
  if (size_t const accept_count = terminal_state_machine_tables_.get_state_final_id_count(state_nr_cur); accept_count > 0) {
   for (size_t i = 0; i < accept_count; ++i) {
    FinalIdType const accept_ = terminal_state_machine_tables_.get_state_final_id(state_nr_cur, i);
    MemoTable::Match match{
     ._length = cursor_ - cursor_start,
     ._matching_subclauses = {},
    };

    memo_table_.insert(MemoTable::Key{._clause = &pika_tables_.fetch_clause(accept_), ._position = cursor_start}, match, clause_queue_, pika_tables_);
   }
  }

  if (cursor_ == input_.size()) {
   break;
  }

  state_nr_cur = terminal_state_machine_tables_.get_state_nr_next(state_nr_cur, input_[cursor_]);
 }
}

}  // namespace

auto PikaParser::populate_memo_table(std::string_view input_, PikaTablesBase const& pika_tables_) -> MemoTable {
 MemoTable memo_table;
 ClauseQueue clause_queue;

 for (size_t cursor = input_.size(); cursor-- > 0;) {
  scan_terminals(pika_tables_.get_terminal_state_machine_tables(), cursor, memo_table, clause_queue, input_, pika_tables_);

  while (!clause_queue.empty()) {
   ClauseQueueItem item = clause_queue.top();
   clause_queue.pop();

   MemoTable::Key key{._clause = item._clause, ._position = cursor};
   memo_table.insert(key, ClauseMatcher::match(memo_table, key, input_, pika_tables_), clause_queue, pika_tables_);
  }
 }

 return memo_table;
}

}  // namespace pmt::rt
