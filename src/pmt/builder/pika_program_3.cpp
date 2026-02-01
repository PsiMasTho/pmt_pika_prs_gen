#include "pmt/builder/pika_program.hpp"

#include "pmt/container/bitset.hpp"
#include "pmt/unreachable.hpp"

#include <cassert>
#include <span>

namespace pmt::builder {
using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;
namespace {

struct Locals {
 Bitset _solved;
 std::span<ExtendedClause> _clauses;
 bool _changed;
};

void mark(Locals& locals_, ClauseBase::IdType clause_id_, bool can_match_zero_) {
 if (locals_._solved.get(clause_id_)) {
  return;
 }
 locals_._solved.set(clause_id_, true);
 locals_._clauses[clause_id_]._can_match_zero = can_match_zero_;
 locals_._changed = true;
}

void handle_sequence(Locals& locals_, ClauseBase::IdType clause_id_) {
 if (locals_._clauses[clause_id_].get_child_id_count() == 0) {
  mark(locals_, clause_id_, true);
 }
 bool saw_unsolved = false;
 size_t j = 0;
 for (; j < locals_._clauses[clause_id_].get_child_id_count(); ++j) {
  ClauseBase::IdType const child_id = locals_._clauses[clause_id_].get_child_id_at(j);
  if (!locals_._solved.get(child_id)) {
   saw_unsolved = true;
   continue;
  }
  if (locals_._clauses[child_id].can_match_zero() == false) {
   mark(locals_, clause_id_, false);
   break;
  }
 }
 if (!locals_._solved.get(clause_id_) && j == locals_._clauses[clause_id_].get_child_id_count() && !saw_unsolved) {
  mark(locals_, clause_id_, true);
 }
}

void handle_choice(Locals& locals_, ClauseBase::IdType clause_id_) {
 if (locals_._clauses[clause_id_].get_child_id_count() == 0) {
  mark(locals_, clause_id_, true);
 }

 bool saw_unsolved = false;
 size_t j = 0;
 for (; j < locals_._clauses[clause_id_].get_child_id_count(); ++j) {
  ClauseBase::IdType const child_id = locals_._clauses[clause_id_].get_child_id_at(j);
  if (!locals_._solved.get(child_id)) {
   saw_unsolved = true;
   continue;
  }
  if (locals_._clauses[child_id].can_match_zero() && j + 1 < locals_._clauses[clause_id_].get_child_id_count()) {
   throw std::runtime_error("Invalid grammar: Choice has alternative that can match zero in non-final position");
  }
  if (locals_._clauses[child_id].can_match_zero() == true) {
   mark(locals_, clause_id_, true);
   break;
  }
 }
 if (!locals_._solved.get(clause_id_) && j == locals_._clauses[clause_id_].get_child_id_count() && !saw_unsolved) {
  mark(locals_, clause_id_, false);
 }
}

void handle_charset_literal(Locals& locals_, ClauseBase::IdType clause_id_) {
 mark(locals_, clause_id_, false);
}

void handle_identifier(Locals& locals_, ClauseBase::IdType clause_id_) {
 if (locals_._solved.get(locals_._clauses[clause_id_].get_child_id_at(0))) {
  mark(locals_, clause_id_, locals_._clauses[locals_._clauses[clause_id_].get_child_id_at(0)].can_match_zero());
 }
}

void handle_one_or_more(Locals& locals_, ClauseBase::IdType clause_id_) {
 if (locals_._solved.get(locals_._clauses[clause_id_].get_child_id_at(0))) {
  if (locals_._clauses[locals_._clauses[clause_id_].get_child_id_at(0)].can_match_zero()) {
   throw std::runtime_error("Invalid grammar: OneOrMore can match zero");
  }
  mark(locals_, clause_id_, false);
 }
}

}  // namespace

void PikaProgram::determine_can_match_zero() {
 Locals locals{
  ._solved = Bitset(_clauses.size(), false),
  ._clauses = _clauses,
  ._changed = true,
 };

 while (locals._changed) {
  locals._changed = false;

  for (std::size_t i = _clauses.size(); i-- > 0;) {
   switch (_clauses[i].get_tag()) {
    case ClauseBase::Tag::Sequence: {
     handle_sequence(locals, i);
    } break;
    case ClauseBase::Tag::Choice: {
     handle_choice(locals, i);
    } break;
    case ClauseBase::Tag::CharsetLiteral: {
     handle_charset_literal(locals, i);
    } break;
    case ClauseBase::Tag::Identifier: {
     handle_identifier(locals, i);
    } break;
    case ClauseBase::Tag::OneOrMore: {
     handle_one_or_more(locals, i);
    } break;
    case ClauseBase::Tag::NegativeLookahead:
    case ClauseBase::Tag::Epsilon:
     mark(locals, i, true);
     break;
    default:
     pmt::unreachable();
   }
  }
 }

 assert(locals._solved.all() && "FAILED TO DETERMINE CAN_MATCH_ZERO");
}

}  // namespace pmt::builder