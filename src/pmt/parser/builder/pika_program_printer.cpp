#include "pmt/parser/builder/pika_program_printer.hpp"

#include "pmt/parser/builder/pika_program.hpp"
#include "pmt/parser/clause_base.hpp"
#include "pmt/parser/grammar/util.hpp"

#include <string>

namespace pmt::parser::builder {
using namespace pmt::parser::grammar;

namespace {
static void print_clause_block(PikaProgramPrinter::Args& args_, ClauseBase::IdType clause_id_) {
 ClauseBase const& clause = args_._program.fetch_clause(clause_id_);

 // Header: the clause id, then a braced block
 args_._out << clause_id_ << " {\n";

 // kind
 args_._out << "  kind: " << ClauseBase::tag_to_string(clause.get_tag()) << ";\n";

 // children: ascending list of child clause IDs or the literal
 if (clause.get_tag() == ClauseBase::Tag::Literal) {
  args_._out << "  literal: " << literal_sequence_to_printable_string(args_._program.fetch_lit_seq(clause.get_child_id_at(0)));
 } else {
  std::string delim;
  args_._out << "  children: [";
  for (size_t i = 0; i < clause.get_child_id_count(); ++i) {
   args_._out << std::exchange(delim, ", ") << clause.get_child_id_at(i);
  }
  args_._out << "];\n";
 }

 // rules:
 {
  args_._out << "  rules: [";
  std::string delim;
  for (size_t i = 0; i < clause.get_registered_rule_id_count(); ++i) {
   auto rid = clause.get_registered_rule_id_at(i);
   auto const& rp = args_._program.fetch_rule_parameters(rid);
   args_._out << std::exchange(delim, ", ") << rp._display_name;
  }
  args_._out << "];\n";
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
