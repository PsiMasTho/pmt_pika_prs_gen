#include "pmt/builder/pika_tables.hpp"

#include "pmt/container/bitset.hpp"
#include "pmt/unreachable.hpp"

#include <set>
#include <span>

namespace pmt::builder {
using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;
namespace {

struct Locals {
 Bitset _solved;
 std::span<ExtendedClause> _clauses;
 std::span<RuleParameters> _rule_parameters;
 std::vector<IdType> _closest_rule_ids;
 bool _changed;
};

void mark(Locals& locals_, IdType clause_id_, bool can_match_zero_) {
 if (locals_._solved.get(clause_id_)) {
  return;
 }
 locals_._solved.set(clause_id_, true);
 locals_._clauses[clause_id_]._can_match_zero = can_match_zero_;
 locals_._changed = true;
}

void handle_sequence(Locals& locals_, IdType clause_id_) {
 if (locals_._clauses[clause_id_].get_child_id_count() == 0) {
  mark(locals_, clause_id_, true);
 }
 bool saw_unsolved = false;
 size_t j = 0;
 for (; j < locals_._clauses[clause_id_].get_child_id_count(); ++j) {
  IdType const child_id = locals_._clauses[clause_id_].get_child_id_at(j);
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

void handle_choice(Locals& locals_, IdType clause_id_) {
 if (locals_._clauses[clause_id_].get_child_id_count() == 0) {
  mark(locals_, clause_id_, true);
 }

 bool saw_unsolved = false;
 size_t j = 0;
 for (; j < locals_._clauses[clause_id_].get_child_id_count(); ++j) {
  IdType const child_id = locals_._clauses[clause_id_].get_child_id_at(j);
  if (!locals_._solved.get(child_id)) {
   saw_unsolved = true;
   continue;
  }
  if (locals_._clauses[child_id].can_match_zero() && j + 1 < locals_._clauses[clause_id_].get_child_id_count()) {
   std::string msg = "Invalid grammar: Choice has a non final alternative that can match zero input";
   IdType const closest_rule_id = locals_._closest_rule_ids[clause_id_];
   if (closest_rule_id != ReservedIds::IdInvalid) {
    msg += " in rule $" + locals_._rule_parameters[closest_rule_id]._display_name;
   }
   throw std::runtime_error(msg);
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

void handle_charset_literal(Locals& locals_, IdType clause_id_) {
 mark(locals_, clause_id_, false);
}

void handle_identifier(Locals& locals_, IdType clause_id_) {
 if (locals_._solved.get(locals_._clauses[clause_id_].get_child_id_at(0))) {
  mark(locals_, clause_id_, locals_._clauses[locals_._clauses[clause_id_].get_child_id_at(0)].can_match_zero());
 }
}

auto determine_closest_rule_ids(std::span<ExtendedClause const> clauses_) -> std::vector<IdType> {
 struct PendingItem {
  IdType _cur_clause_id;
  IdType _closest_rule_id;
 };

 std::vector<IdType> ret(clauses_.size(), ReservedIds::IdInvalid);
 Bitset visited(clauses_.size(), false);

 std::vector<PendingItem> pending{PendingItem{._cur_clause_id = 0, ._closest_rule_id = ReservedIds::IdInvalid}};

 while (!pending.empty()) {
  auto [cur_clause_id, closest_rule_id] = pending.back();
  pending.pop_back();

  ClauseBase const& cur_clause = clauses_[cur_clause_id];

  closest_rule_id = (cur_clause.get_tag() == ClauseBase::Tag::Identifier) ? cur_clause.get_rule_id() : closest_rule_id;
  ret[cur_clause_id] = closest_rule_id;
  visited.set(cur_clause_id, true);

  for (size_t i = 0; i < cur_clause.get_child_id_count(); ++i) {
   IdType const child_id = cur_clause.get_child_id_at(i);
   if (!visited.get(child_id)) {
    pending.emplace_back(child_id, closest_rule_id);
   }
  }
 }

 return ret;
}

auto generate_unsolved_error_msg(Locals const& locals_) -> std::string {
 static constexpr size_t const MaxUnsolvedRulesToReport = 4;

 Bitset unsolved = locals_._solved.clone_not();
 std::set<std::string> unsolved_rules;
 bool truncated = false;

 while (unsolved.any()) {
  size_t const clause_id = (unsolved.size() - unsolved.countr(false) - 1);
  unsolved.resize(clause_id);
  IdType const closest_rule_id = locals_._closest_rule_ids[clause_id];
  if (closest_rule_id == ReservedIds::IdInvalid) {
   continue;
  }
  std::string const& rule_name = locals_._rule_parameters[closest_rule_id]._display_name;
  if (unsolved_rules.contains(rule_name)) {
   continue;
  }
  if (unsolved_rules.size() < MaxUnsolvedRulesToReport) {
   unsolved_rules.insert(rule_name);
  } else {
   truncated = true;
  }
 }

 size_t const items_reported = unsolved_rules.size() + (truncated ? 1 : 0);

 std::string msg = "Invalid grammar: Failed to determine can_match_zero";
 if (!unsolved_rules.empty()) {
  msg += " in rule";
  msg += (items_reported > 1) ? "s: " : ": ";
  for (size_t i = 0; std::string const& rule_name : unsolved_rules) {
   if (i > 0) {
    msg += ", ";
   }
   msg += "$" + rule_name;
   ++i;
  }
  if (truncated) {
   msg += ", ...";
  }
 }

 return msg;
}

}  // namespace

void PikaTables::determine_can_match_zero() {
 Locals locals{
  ._solved = Bitset(_clauses.size(), false),
  ._clauses = _clauses,
  ._rule_parameters = _rule_parameters,
  ._closest_rule_ids = determine_closest_rule_ids(_clauses),
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
    case ClauseBase::Tag::NegativeLookahead:
    case ClauseBase::Tag::Epsilon:
     mark(locals, i, true);
     break;
    default:
     pmt::unreachable();
   }
  }
 }

 if (!locals._solved.all()) {
  throw std::runtime_error(generate_unsolved_error_msg(locals));
 }
}

}  // namespace pmt::builder
