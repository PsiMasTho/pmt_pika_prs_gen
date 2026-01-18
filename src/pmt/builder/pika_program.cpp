#include "pmt/builder/pika_program.hpp"

#include "pmt/builder/state_machine.hpp"
#include "pmt/builder/state_machine_determinizer.hpp"
#include "pmt/container/bitset.hpp"
#include "pmt/meta/grammar.hpp"
#include "pmt/meta/rule_expression.hpp"
#include "pmt/rt/pika_program_base.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/overloaded.hpp"

#include <cassert>
#include <unordered_set>

namespace pmt::builder {
using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;

namespace {

enum : ClauseBase::IdType {
 IdTmpEpsilon = std::numeric_limits<ClauseBase::IdType>::max(),
};

auto charset_literals_to_state_machine(std::span<CharsetLiteral const> charset_literals_, std::span<ClauseBase::IdType const> charset_literal_clause_unique_ids_) -> StateMachine {
 StateMachine ret;
 ret.create_new_state();  // StateNrStart

 for (size_t idx = 0; CharsetLiteral const& charset_literal : charset_literals_) {
  StateNrType state_nr_prev = ret.create_new_state();
  ret.get_state(StateNrStart)->add_epsilon_transition(state_nr_prev);

  for (size_t i = 1; i < charset_literal.size() + 1; ++i) {
   StateNrType state_nr_cur = ret.create_new_state();

   if (!charset_literal.get_symbol_set_at(i - 1).empty()) {
    charset_literal.get_symbol_set_at(i - 1).for_each_interval([&](Interval<SymbolType> interval_) { ret.get_state(state_nr_prev)->add_symbol_transitions(interval_, state_nr_cur); });
   } else {
    ret.get_state(state_nr_prev)->add_epsilon_transition(state_nr_cur);
   }

   state_nr_prev = state_nr_cur;
  }

  ret.get_state(state_nr_prev)->add_final_id(charset_literal_clause_unique_ids_[idx++]);
 }

 return StateMachineDeterminizer::determinize(ret, StateNrStart);
}

using CharsetLiteralFetch = decltype(pmt::util::Overloaded{[](std::vector<CharsetLiteral> const& charset_literals_, size_t index_) { return index_ < charset_literals_.size() ? &charset_literals_[index_] : nullptr; },
                                                           [](std::vector<CharsetLiteral> const&, CharsetLiteral const& item_) {
                                                            return &item_;
                                                           }});

class CharsetLiteralIndirectHash {
 std::vector<CharsetLiteral> const& _charset_literals;

public:
 using is_transparent = void;  // NOLINT

 explicit CharsetLiteralIndirectHash(std::vector<CharsetLiteral> const& charset_literals_)
  : _charset_literals(charset_literals_) {
 }

 auto operator()(auto const& item_) const -> size_t {
  return CharsetLiteralFetch{}(_charset_literals, item_)->hash();
 }
};

class CharsetLiteralIndirectEq {
 std::vector<CharsetLiteral> const& _charset_literals;

public:
 using is_transparent = void;  // NOLINT

 explicit CharsetLiteralIndirectEq(std::vector<CharsetLiteral> const& charset_literals_)
  : _charset_literals(charset_literals_) {
 }

 auto operator()(auto const& lhs_, auto const& rhs_) const -> bool {
  CharsetLiteral const* lhs = CharsetLiteralFetch{}(_charset_literals, lhs_);
  CharsetLiteral const* rhs = CharsetLiteralFetch{}(_charset_literals, rhs_);
  if (lhs == nullptr || rhs == nullptr) {
   return false;
  }
  return *lhs == *rhs;
 }
};

class CharsetLiteralsCached {
 std::vector<CharsetLiteral>& _charset_literals;
 std::unordered_set<size_t, CharsetLiteralIndirectHash, CharsetLiteralIndirectEq> _index_cache;

public:
 explicit CharsetLiteralsCached(std::vector<CharsetLiteral>& charset_literals_)
  : _charset_literals(charset_literals_)
  , _index_cache(0, CharsetLiteralIndirectHash(_charset_literals), CharsetLiteralIndirectEq(_charset_literals)) {
 }

 // Returns <id, is_new>
 auto add_or_get_id(CharsetLiteral const& charset_literal_) -> std::pair<ClauseBase::IdType, bool> {
  auto const itr = _index_cache.find(charset_literal_);
  if (itr != _index_cache.end()) {
   return {*itr, false};
  }

  ClauseBase::IdType const idx = _charset_literals.size();

  _charset_literals.push_back(charset_literal_);
  _index_cache.insert(idx);
  return {idx, true};
 }
};

void renumber_clause_ids(std::vector<ExtendedClause>& clauses_, std::unordered_map<ClauseBase::IdType, ClauseBase::IdType> const& id_mapping_) {
 for (ExtendedClause& clause : clauses_) {
  for (ClauseBase::IdType& child_id : clause._child_ids) {
   auto const itr = id_mapping_.find(child_id);
   assert(itr != id_mapping_.end());
   child_id = itr->second;
  }
 }
}

}  // namespace

ExtendedClause::ExtendedClause(Tag tag_, ClauseBase::IdType id_)
 : _tag(tag_)
 , _id(id_) {
}

auto ExtendedClause::get_tag() const -> Tag {
 return _tag;
}

auto ExtendedClause::get_id() const -> IdType {
 return _id;
}

auto ExtendedClause::get_child_id_at(size_t idx_) const -> ClauseBase::IdType {
 assert(idx_ < _child_ids.size());
 return _child_ids[idx_];
}

auto ExtendedClause::get_child_id_count() const -> size_t {
 return _child_ids.size();
}

auto ExtendedClause::get_literal_id() const -> IdType {
 return _literal_id;
}

auto ExtendedClause::get_rule_id() const -> IdType {
 return _rule_id;
}

auto ExtendedClause::get_seed_parent_id_at(size_t idx_) const -> ClauseBase::IdType {
 assert(idx_ < _seed_parent_ids.size());
 return _seed_parent_ids[idx_];
}

auto ExtendedClause::get_seed_parent_count() const -> size_t {
 return _seed_parent_ids.size();
}

auto ExtendedClause::can_match_zero() const -> bool {
 return _can_match_zero;
}

PikaProgram::PikaProgram(Grammar const& grammar_)
 : _id_table(grammar_.get_id_table()) {
 initialize(grammar_);
 determine_can_match_zero();
 determine_seed_parents();
}

auto PikaProgram::fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& {
 assert(clause_id_ < get_clause_count());
 return _clauses[clause_id_];
}

auto PikaProgram::get_clause_count() const -> size_t {
 return _clauses.size();
}

auto PikaProgram::fetch_rule_parameters(ClauseBase::IdType rule_id_) const -> RuleParametersBase const& {
 return _rule_parameters[rule_id_];
}

auto PikaProgram::get_rule_count() const -> size_t {
 return _rule_parameters.size();
}

auto PikaProgram::get_terminal_state_machine_tables() const -> StateMachineTablesBase const& {
 return _literal_state_machine_tables;
}

auto PikaProgram::fetch_literal(ClauseBase::IdType literal_id_) const -> CharsetLiteral const& {
 assert(literal_id_ < _literals.size());
 return _literals[literal_id_];
}

auto PikaProgram::get_id_table() const -> pmt::meta::IdTable const& {
 return _id_table;
}

auto PikaProgram::get_literal_state_machine_tables() const -> StateMachineTables const& {
 return _literal_state_machine_tables;
}

void PikaProgram::initialize(Grammar const& grammar_) {
 std::vector<ClauseBase::IdType> charset_literal_clause_ids;
 CharsetLiteralsCached charset_literals_cached(_literals);

 IntervalSet<ClauseBase::IdType> clauses_with_epsilon_children;
 std::unordered_map<RuleExpression const*, ClauseBase::IdType> visited;
 std::function<ClauseBase::IdType(RuleExpression const*)> recursive_worker = [&](RuleExpression const* expr_) -> ClauseBase::IdType {
  if (auto const itr = visited.find(expr_); itr != visited.end()) {
   return itr->second;
  }

  if (expr_->get_tag() == ClauseBase::Tag::CharsetLiteral) {
   auto const [literal_id, is_new] = charset_literals_cached.add_or_get_id(expr_->get_charset_literal());
   ClauseBase::IdType const clause_id = is_new ? _clauses.size() : charset_literal_clause_ids[literal_id];
   if (is_new) {
    charset_literal_clause_ids.push_back(clause_id);
    _clauses.emplace_back(expr_->get_tag(), clause_id);
   }

   _clauses[clause_id]._literal_id = literal_id;
   visited[expr_] = clause_id;
   return clause_id;
  } else if (expr_->get_tag() == ClauseBase::Tag::Epsilon) {
   ClauseBase::IdType const clause_id = IdTmpEpsilon;
   visited[expr_] = clause_id;
   return clause_id;
  }

  ClauseBase::IdType const clause_id = _clauses.size();
  visited[expr_] = clause_id;
  _clauses.emplace_back(expr_->get_tag(), clause_id);

  auto const recurse = [&](RuleExpression const* expr_) {
   ClauseBase::IdType const child_id = recursive_worker(expr_);
   if (child_id == IdTmpEpsilon) {
    clauses_with_epsilon_children.insert(Interval(clause_id));
   }
   return child_id;
  };

  switch (expr_->get_tag()) {
   case ClauseBase::Tag::Sequence: {
    for (size_t i = 0; i < expr_->get_children_size(); ++i) {
     ClauseBase::IdType const child_id = recurse(expr_->get_child_at(i));
     _clauses[clause_id]._child_ids.push_back(child_id);
    }
   } break;
   case ClauseBase::Tag::Choice: {
    for (size_t i = 0; i < expr_->get_children_size(); ++i) {
     ClauseBase::IdType const child_id = recurse(expr_->get_child_at(i));
     _clauses[clause_id]._child_ids.push_back(child_id);
    }
   } break;
   case ClauseBase::Tag::Hidden: {
    ClauseBase::IdType const child_id = recurse(expr_->get_child_at_front());
    _clauses[clause_id]._child_ids.push_back(child_id);
   } break;
   case ClauseBase::Tag::OneOrMore: {
    ClauseBase::IdType const child_id = recurse(expr_->get_child_at_front());
    _clauses[clause_id]._child_ids.push_back(child_id);
   } break;
   case ClauseBase::Tag::NegativeLookahead: {
    ClauseBase::IdType const child_id = recurse(expr_->get_child_at_front());
    _clauses[clause_id]._child_ids.push_back(child_id);
   } break;
   case ClauseBase::Tag::CharsetLiteral:
    break;
   case ClauseBase::Tag::Identifier: {
    Rule const* rule = grammar_.get_rule(expr_->get_identifier());
    if (rule == nullptr) {
     throw std::runtime_error("Unknown rule identifier '" + expr_->get_identifier() + "'");  // -$ Todo $- better error reporting
    }
    ClauseBase::IdType const child_id = recurse(rule->_definition.get());
    _clauses[clause_id]._child_ids.push_back(child_id);
    _clauses[clause_id]._rule_id = _rule_parameters.size();

    _rule_parameters.emplace_back();
    _rule_parameters.back()._display_name = rule->_parameters._display_name;
    _rule_parameters.back()._id_string = rule->_parameters._id_string;
    _rule_parameters.back()._id_value = _id_table.string_to_id(rule->_parameters._id_string);
    _rule_parameters.back()._merge = rule->_parameters._merge;
    _rule_parameters.back()._unpack = rule->_parameters._unpack;
    _rule_parameters.back()._hide = rule->_parameters._hide;
   } break;
   case ClauseBase::Tag::Epsilon:
   default:
    pmt::unreachable();
  }

  return clause_id;
 };

 RuleExpression::UniqueHandle const start_expr = grammar_.get_start_expression();
 recursive_worker(start_expr.get());

 // Replace temporary epsilon children with a single shared epsilon clause
 if (!clauses_with_epsilon_children.empty()) {
  ClauseBase::IdType const epsilon_clause_id = _clauses.size();
  _clauses.emplace_back(ClauseBase::Tag::Epsilon, epsilon_clause_id);
  clauses_with_epsilon_children.for_each_key([&](ClauseBase::IdType clause_id_) {
   ExtendedClause& clause = _clauses[clause_id_];
   for (ClauseBase::IdType& child_id : clause._child_ids) {
    if (child_id == IdTmpEpsilon) {
     child_id = epsilon_clause_id;
    }
   }
  });
 }

 // Build the terminal state machine from the charset literals
 _literal_state_machine_tables = StateMachineTables(charset_literals_to_state_machine(_literals, charset_literal_clause_ids));
}

void PikaProgram::determine_can_match_zero() {
 Bitset solved(_clauses.size());
 bool changed = true;

 auto const mark = [&](ClauseBase::IdType clause_id_, bool can_match_zero_) {
  if (solved.get(clause_id_)) {
   return;
  }
  solved.set(clause_id_, true);
  _clauses[clause_id_]._can_match_zero = can_match_zero_;
  changed = true;
 };

 while (changed) {
  changed = false;

  for (std::size_t i = _clauses.size(); i-- > 0;) {
   switch (_clauses[i].get_tag()) {
    case ClauseBase::Tag::Sequence:
    case ClauseBase::Tag::Choice: {
     bool const is_sequence = _clauses[i].get_tag() == ClauseBase::Tag::Sequence;
     if (_clauses[i].get_child_id_count() == 0) {
      mark(i, true);
     }

     bool saw_unsolved = false;
     size_t j = 0;
     for (; j < _clauses[i].get_child_id_count(); ++j) {
      ClauseBase::IdType const child_id = _clauses[i].get_child_id_at(j);
      if (!solved.get(child_id)) {
       saw_unsolved = true;
       continue;
      }
      if (_clauses[child_id].can_match_zero() && !is_sequence && j + 1 < _clauses[i].get_child_id_count()) {
       // Choice has alternative that can match zero in non-final position
       // -$ Todo $- make this error reporting nicer
       throw std::runtime_error("Invalid grammar");
      }
      if (_clauses[child_id].can_match_zero() == !is_sequence) {
       mark(i, !is_sequence);
       break;
      }
     }
     if (!solved.get(i) && j == _clauses[i].get_child_id_count() && !saw_unsolved) {
      mark(i, is_sequence);
     }
    } break;

    case ClauseBase::Tag::CharsetLiteral: {
     mark(i, false);
    } break;
    case ClauseBase::Tag::Identifier:
    case ClauseBase::Tag::Hidden:
    case ClauseBase::Tag::OneOrMore: {
     if (solved.get(_clauses[i].get_child_id_at(0))) {
      mark(i, _clauses[_clauses[i].get_child_id_at(0)].can_match_zero());
     }
    } break;
    case ClauseBase::Tag::NegativeLookahead: {
     ClauseBase::IdType const child_id = _clauses[i].get_child_id_at(0);
     if (solved.get(child_id) && _clauses[child_id].can_match_zero()) {
      throw std::runtime_error("Invalid grammar");
     }
    }
    case ClauseBase::Tag::Epsilon:
     mark(i, true);
     break;
    default:
     pmt::unreachable();
   }
  }
 }

 assert(solved.all() && "FAILED TO DETERMINE CAN_MATCH_ZERO");
}

void PikaProgram::determine_seed_parents() {
 Bitset visited(_clauses.size());
 std::function<void(ClauseBase::IdType)> recursive_worker = [&](ClauseBase::IdType clause_id_) {
  if (visited.exchange(clause_id_, true)) {
   return;
  }

  ExtendedClause& clause = _clauses[clause_id_];

  switch (clause.get_tag()) {
   case ClauseBase::Tag::Sequence: {
    if (clause.get_child_id_count() != 0) {
     ClauseBase::IdType const first_child_id = clause.get_child_id_at(0);
     _clauses[first_child_id]._seed_parent_ids.push_back(clause_id_);
    }
    for (size_t i = 0; i + 1 < clause.get_child_id_count(); ++i) {
     if (this->fetch_clause(clause.get_child_id_at(i)).can_match_zero()) {
      ClauseBase::IdType const next_child_id = clause.get_child_id_at(i + 1);
      _clauses[next_child_id]._seed_parent_ids.push_back(clause_id_);
     } else {
      break;
     }
    }

   } break;
   case ClauseBase::Tag::Identifier:
   case ClauseBase::Tag::Hidden:
   case ClauseBase::Tag::OneOrMore:
   case ClauseBase::Tag::Choice:
   case ClauseBase::Tag::NegativeLookahead: {
    for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
     ClauseBase::IdType const child_id = clause.get_child_id_at(i);
     _clauses[child_id]._seed_parent_ids.push_back(clause_id_);
    }
   } break;
   default:
    pmt::unreachable();
   case ClauseBase::Tag::Epsilon:
   case ClauseBase::Tag::CharsetLiteral:
    break;
  }

  // Evaluate children
  for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
   recursive_worker(clause.get_child_id_at(i));
  }
 };

 if (!_clauses.empty()) {
  recursive_worker(0);
 }
}

}  // namespace pmt::builder
