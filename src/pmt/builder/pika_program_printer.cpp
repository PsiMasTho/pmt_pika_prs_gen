#include "pmt/builder/pika_program_printer.hpp"

#include "pmt/builder/pika_program.hpp"
#include "pmt/meta/literal_to_str.hpp"
#include "pmt/rt/clause_base.hpp"

#include <string>

namespace pmt::builder {
using namespace pmt::meta;
using namespace pmt::rt;
using namespace pmt::container;

namespace {

static void print_clause_block(PikaProgram const& program_, std::ostream& out_, ClauseBase::IdType clause_id_) {
 ClauseBase const& clause = program_.fetch_clause(clause_id_);

 // Header: the clause id, then a braced block
 out_ << clause_id_ << " {\n";

 // kind
 out_ << "  kind: " << ClauseBase::tag_to_string(clause.get_tag()) << ";\n";

 // children: ascending list of child clause IDs or the literal
 if (clause.get_tag() == ClauseBase::Tag::CharsetLiteral) {
  out_ << "  literal: " << charset_literal_to_grammar_string(program_.fetch_literal(clause.get_literal_id())) << ";\n";
 } else {
  std::string delim;
  out_ << "  children: [";
  for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
   out_ << std::exchange(delim, ", ") << clause.get_child_id_at(i);
  }
  out_ << "];\n";
 }

 // identifier:
 if (clause.get_tag() == ClauseBase::Tag::Identifier) {
  out_ << "  identifier: " << program_.fetch_rule_parameters(clause.get_rule_id()).get_display_name() << ";\n";
 }

 // seed parents:
 {
  out_ << "  seed_parents: [";
  std::string delim;
  for (size_t i = 0; i < clause.get_seed_parent_count(); ++i) {
   out_ << std::exchange(delim, ", ") << clause.get_seed_parent_id_at(i);
  }
  out_ << "];\n";
 }

 // canMatchZeroChars:
 out_ << "  canMatchZeroChars: " << (clause.can_match_zero() ? "true" : "false") << "\n";

 out_ << "}\n";
}

void print_clauses(PikaProgram const& program_, std::ostream& out_) {
 for (size_t i = 0; i < program_.get_clause_count(); ++i) {
  print_clause_block(program_, out_, i);
 }
 out_ << std::endl;
}

}  // namespace

void PikaProgramPrinter::print(PikaProgram const& program_, std::ostream& out_) {
 print_clauses(program_, out_);
}

}  // namespace pmt::builder