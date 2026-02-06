#include "pmt/builder/pika_tables_to_str.hpp"

#include "pmt/builder/pika_tables.hpp"
#include "pmt/meta/literal_to_str.hpp"
#include "pmt/rt/clause_base.hpp"

namespace pmt::builder {
using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;

namespace {

auto clause_block_to_str(PikaTables const& program_, IdType clause_id_) -> std::string {
 std::string ret;

 ClauseBase const& clause = program_.fetch_clause(clause_id_);

 ret += std::to_string(clause_id_) + " {\n";

 ret += "  kind: " + ClauseBase::tag_to_string(clause.get_tag()) + ";\n";

 if (clause.get_tag() == ClauseBase::Tag::CharsetLiteral) {
  ret += "  literal: " + charset_literal_to_grammar_string(program_.fetch_literal(clause.get_literal_id())) + ";\n";
 } else {
  std::string delim;
  ret += "  children: [";
  for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
   ret += std::exchange(delim, ", ") + std::to_string(clause.get_child_id_at(i));
  }
  ret += "];\n";
 }

 // identifier:
 if (clause.get_tag() == ClauseBase::Tag::Identifier) {
  ret += "  identifier: " + std::string(program_.fetch_rule_parameters(clause.get_rule_id()).get_display_name()) + ";\n";
 }

 // seed parents:
 {
  ret += "  seed_parents: [";
  std::string delim;
  for (size_t i = 0; i < clause.get_seed_parent_count(); ++i) {
   ret += std::exchange(delim, ", ") + std::to_string(clause.get_seed_parent_id_at(i));
  }
  ret += "];\n";
 }

 // canMatchZeroChars:
 ret += std::string("  can_match_zero: ") + (clause.can_match_zero() ? "true" : "false") + "\n";

 return ret + "}\n";
}

}  // namespace

auto pika_tables_to_string(PikaTables const& program_) -> std::string {
 std::string ret;
 for (size_t i = 0; i < program_.get_clause_count(); ++i) {
  ret += clause_block_to_str(program_, i);
 }
 return ret + "\n";
}

}  // namespace pmt::builder