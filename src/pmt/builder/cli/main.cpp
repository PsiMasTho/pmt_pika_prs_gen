#include "pmt/builder/cli/args.hpp"
#include "pmt/builder/id_emitter.hpp"
#include "pmt/builder/pika_tables.hpp"
#include "pmt/builder/pika_tables_emitter.hpp"
#include "pmt/builder/pika_tables_to_str.hpp"
#include "pmt/builder/terminal_dotfile_emitter.hpp"
#include "pmt/meta/grammar_to_str.hpp"
#include "pmt/meta/load_grammar.hpp"
#include "pmt/util/read_file.hpp"

#include "pmt/rt/ast.hpp"
#include "pmt/rt/ast_to_str.hpp"
#include "pmt/rt/pika_parser.hpp"

#include <fstream>
#include <iostream>

using namespace pmt::rt;

namespace {
void debug_print_memo_table(MemoTable const& memo_table_, std::string_view input_, PikaTablesBase const& pika_program_) {
 std::cerr << "MemoTable matches: " << memo_table_.get_match_count() << '\n';
 for (MemoTable::IndexType match_index = 0; match_index < memo_table_.get_match_count(); ++match_index) {
  MemoTable::Match const& match = memo_table_.get_match_by_index(match_index);
  MemoTable::Key const& key = memo_table_.get_key_by_index(match._key_index);
  ClauseBase const* clause = key._clause;
  ClauseBase::Tag const tag = clause->get_tag();

  std::cerr << "match[" << match_index << "] key=" << match._key_index << " clause_id=" << clause->get_id() << " tag=" << ClauseBase::tag_to_string(tag) << " pos=" << key._position << " len=" << match._length;

  if (clause->has_rule_id()) {
   RuleParametersBase const& rule_parameters = pika_program_.fetch_rule_parameters(clause->get_rule_id());
   std::cerr << " rule=" << rule_parameters.get_display_name() << " id=" << rule_parameters.get_id_string();
  } else if (clause->has_literal_id()) {
   std::cerr << " literal_id=" << clause->get_literal_id();
  }

  if (!match._matching_subclauses.empty()) {
   std::cerr << " sub=[";
   for (size_t i = 0; i < match._matching_subclauses.size(); ++i) {
    if (i != 0) {
     std::cerr << ", ";
    }
    MemoTable::IndexType const sub_index = match._matching_subclauses[i];
    if (sub_index == MemoTable::MemoIndexMatchNotFound) {
     std::cerr << "NotFound";
    } else if (sub_index == MemoTable::MemoIndexMatchZeroLength) {
     std::cerr << "ZeroLength";
    } else {
     std::cerr << sub_index;
    }
   }
   std::cerr << "]";
  }

  if (key._position <= input_.size() && key._position + match._length <= input_.size()) {
   std::cerr << " text=\"" << input_.substr(key._position, match._length) << '"';
  }

  std::cerr << '\n';
 }
}
}  // namespace

auto main(int argc_, char const* const* argv_) -> int try {
 pmt::builder::cli::Args args(argc_, argv_);

 std::string const input_grammar = pmt::util::read_file(args._input_grammar_file);
 pmt::meta::Grammar const grammar = pmt::meta::load_grammar(input_grammar);

 if (args._output_grammar_file.has_value()) {
  std::ofstream os(*args._output_grammar_file);
  os << pmt::meta::grammar_to_string(grammar);
 }

 pmt::builder::PikaTables const pika_tables(grammar);
 if (args._output_clauses_file.has_value()) {
  std::ofstream os(*args._output_clauses_file);
  os << pmt::builder::pika_tables_to_string(pika_tables);
 }

 if (args._input_test_file.has_value()) {
  std::string const input_test = pmt::util::read_file(*args._input_test_file);
  pmt::rt::MemoTable const memo_table = PikaParser::populate_memo_table(input_test, pika_tables);
  // debug_print_memo_table(memo_table, input_test, pika_tables);
  Ast::UniqueHandle ast_testfile = PikaParser::memo_table_to_ast(memo_table, input_test, pika_tables);
  if (ast_testfile != nullptr) {
   pmt::rt::AstToString const ast_to_string([&](AstId::IdType id_) { return pika_tables.get_id_table().id_to_string(id_); }, 2);
   std::cout << ast_to_string.to_string(*ast_testfile);
  } else {
   std::cout << "Failed to parse test input.\n";
  }
 }

 {
  std::ofstream output_header(args._pika_tables_output_header_file);
  std::ofstream output_source(args._pika_tables_output_source_file);
  pmt::builder::PikaTablesEmitter pika_program_emitter(pmt::builder::PikaTablesEmitter::Args{
   ._program = pika_tables,
   ._header_include_path = args._pika_tables_header_include_filename,
   ._output_header = output_header,
   ._output_source = output_source,
   ._header_skel = pmt::util::read_file(args._pika_tables_header_skel_file),
   ._source_skel = pmt::util::read_file(args._pika_tables_source_skel_file),
   ._class_name = args._pika_tables_class_name,
   ._namespace_name = args._pika_tables_namespace_name,
  });
  pika_program_emitter.emit();
 }

 {
  std::ofstream output_id_strings(args._id_strings_output_file);
  std::ofstream output_id_constants(args._id_constants_output_file);
  pmt::builder::IdEmitter id_emitter(pmt::builder::IdEmitter::Args{
   ._id_table = pika_tables.get_id_table(),
   ._strings_output_file = output_id_strings,
   ._strings_skel = pmt::util::read_file(args._id_strings_skel_file),
   ._constants_output_file = output_id_constants,
   ._constants_skel = pmt::util::read_file(args._id_constants_skel_file),
  });
  id_emitter.emit();
 }

 if (args._terminal_graph_output_file.has_value() && args._terminal_graph_skel_file.has_value()) {
  std::ofstream output_graph(*args._terminal_graph_output_file);
  pmt::builder::TerminalDotfileEmitter dot_writer(pmt::builder::TerminalDotfileEmitter::Args{
   ._final_id_to_string_fn = [&](FinalIdType idx_) { return std::to_string(idx_); },
   ._skel = pmt::util::read_file(*args._terminal_graph_skel_file),
   ._state_machine = pika_tables.get_literal_state_machine_tables().get_state_machine(),
   ._os_graph = output_graph,
  });
  dot_writer.write_dot();
 }

} catch (std::exception const& e) {
 std::cerr << std::string(e.what()) << '\n';
 return 1;
} catch (...) {
 std::cerr << "Unhandled exception!\n";
 return 1;
}
