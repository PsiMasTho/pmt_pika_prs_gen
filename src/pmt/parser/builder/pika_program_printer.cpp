#include "pmt/parser/builder/pika_program_printer.hpp"

#include "pmt/parser/builder/pika_program.hpp"
#include "pmt/parser/grammar/util.hpp"
#include "pmt/parser/rt/clause_base.hpp"

#include <string>

namespace pmt::parser::builder {
using namespace pmt::parser::grammar;
using namespace pmt::parser::rt;
using namespace pmt::base;
using namespace pmt::sm;

namespace {

static void print_clause_block(PikaProgramPrinter::Args& args_, ClauseBase::IdType clause_id_) {
 ClauseBase const& clause = args_._program.fetch_clause(clause_id_);

 // Header: the clause id, then a braced block
 args_._out << clause_id_ << " {\n";

 // kind
 args_._out << "  kind: " << ClauseBase::tag_to_string(clause.get_tag()) << ";\n";

 // children: ascending list of child clause IDs or the literal
 if (clause.get_tag() == ClauseBase::Tag::CharsetLiteral) {
  args_._out << "  literal: " << charset_literal_to_printable_string(args_._program.fetch_literal(clause.get_literal_id())) << ";\n";
 } else {
  std::string delim;
  args_._out << "  children: [";
  for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
   args_._out << std::exchange(delim, ", ") << clause.get_child_id_at(i);
  }
  args_._out << "];\n";
 }

 // identifier:
 if (clause.get_tag() == ClauseBase::Tag::Identifier) {
  args_._out << "  identifier: " << args_._program.fetch_rule_parameters(clause.get_rule_id()).get_display_name() << ";\n";
 }

 // seed parents:
 {
  args_._out << "  seed_parents: [";
  std::string delim;
  for (size_t i = 0; i < clause.get_seed_parent_count(); ++i) {
   args_._out << std::exchange(delim, ", ") << clause.get_seed_parent_id_at(i);
  }
  args_._out << "];\n";
 }

 // canMatchZeroChars:
 args_._out << "  canMatchZeroChars: " << (clause.can_match_zero() ? "true" : "false") << "\n";

 args_._out << "}\n";
}

void print_clauses(PikaProgramPrinter::Args& args_) {
 for (size_t i = 0; i < args_._program.get_clause_count(); ++i) {
  print_clause_block(args_, i);
 }
 args_._out << std::endl;
}

}  // namespace

void PikaProgramPrinter::print(Args args_) {
 print_clauses(args_);
}

}  // namespace pmt::parser::builder
