#include "pmt/builder/pika_tables.hpp"

#include "pmt/meta/grammar.hpp"
#include "pmt/rt/pika_tables_base.hpp"

#include <cassert>

namespace pmt::builder {
using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;

namespace {}  // namespace

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

PikaTables::PikaTables(Grammar const& grammar_)
 : _id_table(grammar_.get_id_table()) {
 initialize(grammar_);
 determine_can_match_zero();
 determine_seed_parents();
}

auto PikaTables::fetch_clause(ClauseBase::IdType clause_id_) const -> ClauseBase const& {
 assert(clause_id_ < get_clause_count());
 return _clauses[clause_id_];
}

auto PikaTables::get_clause_count() const -> size_t {
 return _clauses.size();
}

auto PikaTables::fetch_rule_parameters(ClauseBase::IdType rule_id_) const -> RuleParametersBase const& {
 return _rule_parameters[rule_id_];
}

auto PikaTables::get_rule_count() const -> size_t {
 return _rule_parameters.size();
}

auto PikaTables::get_terminal_state_machine_tables() const -> StateMachineTablesBase const& {
 return _terminal_state_machine_tables;
}

auto PikaTables::fetch_literal(ClauseBase::IdType literal_id_) const -> CharsetLiteral const& {
 assert(literal_id_ < _literals.size());
 return _literals[literal_id_];
}

auto PikaTables::get_id_table() const -> pmt::meta::IdTable const& {
 return _id_table;
}

auto PikaTables::get_terminal_state_machine_tables_full() const -> StateMachineTables const& {
 return _terminal_state_machine_tables;
}

}  // namespace pmt::builder
