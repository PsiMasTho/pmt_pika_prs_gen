#include "pmt/builder/pika_tables.hpp"

#include "pmt/meta/grammar.hpp"
#include "pmt/meta/rule_expression.hpp"
#include "pmt/sm/state_machine.hpp"
#include "pmt/sm/state_machine_determinizer.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/unique_rac_builder.hpp"

namespace pmt::builder {
using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;

namespace {
auto charset_literals_to_state_machine(std::span<CharsetLiteral const> charset_literals_, std::span<IdType const> charset_literal_clause_unique_ids_) -> pmt::sm::StateMachine {
 pmt::sm::StateMachine ret;
 ret.create_new_state();  // StateNrStart

 for (size_t idx = 0; CharsetLiteral const& charset_literal : charset_literals_) {
  StateNrType state_nr_prev = ret.create_new_state();
  ret.get_state(pmt::sm::StateNrStart)->add_epsilon_transition(state_nr_prev);

  for (size_t i = 1; i < charset_literal.size() + 1; ++i) {
   StateNrType state_nr_cur = ret.create_new_state();

   if (!charset_literal.get_symbol_set_at(i - 1).empty()) {
    charset_literal.get_symbol_set_at(i - 1).for_each_interval([&](Interval<SymbolType> interval_) { ret.get_state(state_nr_prev)->add_symbol_transitions(Interval<pmt::sm::SymbolType>(interval_.get_lower(), interval_.get_upper()), state_nr_cur); });
   } else {
    ret.get_state(state_nr_prev)->add_epsilon_transition(state_nr_cur);
   }

   state_nr_prev = state_nr_cur;
  }

  ret.get_state(state_nr_prev)->add_final_id(charset_literal_clause_unique_ids_[idx++]);
 }

 return pmt::sm::StateMachineDeterminizer::determinize(ret, pmt::sm::StateNrStart);
}

}  // namespace

void PikaTables::initialize(Grammar const& grammar_) {
 std::vector<IdType> charset_literal_clause_ids;
 pmt::util::UniqueRacBuilder<std::vector<CharsetLiteral>> charset_literals_cached(_literals);

 IntervalSet<IdType> clauses_with_tmp_children;
 std::unordered_map<RuleExpression const*, IdType> visited;
 struct Frame {
  RuleExpression const* _expr;
  IdType _clause_id;
  size_t _next_child_idx;
  Rule const* _rule;
  enum class Stage {
   Enter,
   ProcessChildren,
   WaitChild
  } _stage;
 };

 std::vector<Frame> stack;
 RuleExpression::UniqueHandle const start_expr = grammar_.get_start_expression();
 stack.push_back(Frame{start_expr.get(), 0, 0, nullptr, Frame::Stage::Enter});

 IdType last_result = ReservedIds::IdTmpEpsilon;
 bool has_result = false;

 while (!stack.empty()) {
  Frame& frame = stack.back();

  switch (frame._stage) {
   case Frame::Stage::Enter: {
    if (auto const itr = visited.find(frame._expr); itr != visited.end()) {
     last_result = itr->second;
     has_result = true;
     stack.pop_back();
     break;
    }

    ClauseBase::Tag const tag = frame._expr->get_tag();
    if (tag == ClauseBase::Tag::CharsetLiteral) {
     auto const [literal_id, is_new] = charset_literals_cached.insert_and_get_index(frame._expr->get_charset_literal());
     IdType const clause_id = is_new ? _clauses.size() : charset_literal_clause_ids[literal_id];
     if (is_new) {
      charset_literal_clause_ids.push_back(clause_id);
      _clauses.emplace_back(tag, clause_id);
     }
     _clauses[clause_id]._special_id = literal_id;
     visited[frame._expr] = clause_id;
     last_result = clause_id;
     has_result = true;
     stack.pop_back();
     break;
    }

    if (tag == ClauseBase::Tag::Epsilon) {
     IdType const clause_id = ReservedIds::IdTmpEpsilon;
     visited[frame._expr] = clause_id;
     last_result = clause_id;
     has_result = true;
     stack.pop_back();
     break;
    }

    IdType const clause_id = _clauses.size();
    visited[frame._expr] = clause_id;
    _clauses.emplace_back(tag, clause_id);
    frame._clause_id = clause_id;
    frame._next_child_idx = 0;
    frame._rule = nullptr;
    if (tag == ClauseBase::Tag::Identifier) {
     frame._rule = grammar_.get_rule(frame._expr->get_identifier());
     assert(frame._rule != nullptr);
    }
    frame._stage = Frame::Stage::ProcessChildren;
   } break;
   case Frame::Stage::ProcessChildren: {
    ClauseBase::Tag const tag = frame._expr->get_tag();
    RuleExpression const* child_expr = nullptr;
    size_t child_count = 0;

    switch (tag) {
     case ClauseBase::Tag::Sequence:
     case ClauseBase::Tag::Choice:
      child_count = frame._expr->get_children_size();
      if (frame._next_child_idx < child_count) {
       child_expr = frame._expr->get_child_at(frame._next_child_idx);
      }
      break;
     case ClauseBase::Tag::NegativeLookahead:
      child_count = 1;
      if (frame._next_child_idx == 0) {
       child_expr = frame._expr->get_child_at_front();
      }
      break;
     case ClauseBase::Tag::Identifier:
      child_count = 1;
      if (frame._next_child_idx == 0) {
       child_expr = frame._rule->_definition.get();
      }
      break;
     case ClauseBase::Tag::CharsetLiteral:
     case ClauseBase::Tag::Epsilon:
      pmt::unreachable();
      break;
     default:
      pmt::unreachable();
    }

    if (frame._next_child_idx >= child_count) {
     if (tag == ClauseBase::Tag::Identifier) {
      _clauses[frame._clause_id]._special_id = _rule_parameters.size();
      _rule_parameters.emplace_back();
      _rule_parameters.back()._display_name = frame._rule->_parameters._display_name;
      _rule_parameters.back()._id_string = frame._rule->_parameters._id_string;
      _rule_parameters.back()._id_value = _id_table.string_to_id(frame._rule->_parameters._id_string);
      _rule_parameters.back()._merge = frame._rule->_parameters._merge;
      _rule_parameters.back()._unpack = frame._rule->_parameters._unpack;
      _rule_parameters.back()._hide = frame._rule->_parameters._hide;
     }

     last_result = frame._clause_id;
     has_result = true;
     stack.pop_back();
     break;
    }

    frame._stage = Frame::Stage::WaitChild;
    stack.push_back(Frame{child_expr, 0, 0, nullptr, Frame::Stage::Enter});
   } break;
   case Frame::Stage::WaitChild: {
    assert(has_result);
    IdType const child_id = last_result;
    has_result = false;
    if (child_id == ReservedIds::IdTmpEpsilon) {
     clauses_with_tmp_children.insert(Interval(frame._clause_id));
    }
    _clauses[frame._clause_id]._child_ids.push_back(child_id);
    ++frame._next_child_idx;
    frame._stage = Frame::Stage::ProcessChildren;
   } break;
  }
 }

 // Replace temporary epsilon children with a single shared epsilon clause
 if (!clauses_with_tmp_children.empty()) {
  IdType const epsilon_clause_id = _clauses.size();
  _clauses.emplace_back(ClauseBase::Tag::Epsilon, epsilon_clause_id);
  clauses_with_tmp_children.for_each_key([&](IdType clause_id_) {
   ExtendedClause& clause = _clauses[clause_id_];
   for (IdType& child_id : clause._child_ids) {
    if (child_id == ReservedIds::IdTmpEpsilon) {
     child_id = epsilon_clause_id;
    }
   }
  });
 }

 // Build the terminal state machine from the charset literals
 _terminal_state_machine_tables = StateMachineTables(charset_literals_to_state_machine(_literals, charset_literal_clause_ids));
}

}  // namespace pmt::builder