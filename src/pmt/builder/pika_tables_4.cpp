#include "pmt/builder/pika_tables.hpp"

#include <vector>

#include "pmt/container/bitset.hpp"
#include "pmt/unreachable.hpp"

namespace pmt::builder {

using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;

void PikaTables::determine_seed_parents() {
 Bitset visited(_clauses.size());
 if (_clauses.empty()) {
  return;
 }

 std::vector<ClauseBase::IdType> stack;
 stack.push_back(0);

 while (!stack.empty()) {
  ClauseBase::IdType const clause_id = stack.back();
  stack.pop_back();

  if (visited.exchange(clause_id, true)) {
   continue;
  }

  ExtendedClause& clause = _clauses[clause_id];

  switch (clause.get_tag()) {
   case ClauseBase::Tag::Sequence: {
    if (clause.get_child_id_count() != 0) {
     ClauseBase::IdType const first_child_id = clause.get_child_id_at(0);
     _clauses[first_child_id]._seed_parent_ids.push_back(clause_id);
    }
    for (size_t i = 0; i + 1 < clause.get_child_id_count(); ++i) {
     if (this->fetch_clause(clause.get_child_id_at(i)).can_match_zero()) {
      ClauseBase::IdType const next_child_id = clause.get_child_id_at(i + 1);
      _clauses[next_child_id]._seed_parent_ids.push_back(clause_id);
     } else {
      break;
     }
    }

   } break;
   case ClauseBase::Tag::Identifier:
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::NegativeLookahead: {
    for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
     ClauseBase::IdType const child_id = clause.get_child_id_at(i);
     _clauses[child_id]._seed_parent_ids.push_back(clause_id);
    }
   } break;
   default:
    pmt::unreachable();
   case ClauseBase::Tag::Eof:
   case ClauseBase::Tag::Epsilon:
   case ClauseBase::Tag::CharsetLiteral:
    break;
  }

  // Evaluate children (reverse to preserve recursive order).
  for (size_t i = clause.get_child_id_count(); i > 0; --i) {
   stack.push_back(clause.get_child_id_at(i - 1));
  }
 }
}

}  // namespace pmt::builder
