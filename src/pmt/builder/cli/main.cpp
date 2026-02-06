#include "pmt/builder/cli/args.hpp"
#include "pmt/builder/id_emitter.hpp"
#include "pmt/builder/pika_tables.hpp"
#include "pmt/builder/pika_tables_emitter.hpp"
#include "pmt/builder/pika_tables_to_str.hpp"
#include "pmt/builder/terminal_dotfile_emitter.hpp"
#include "pmt/meta/grammar_to_str.hpp"
#include "pmt/meta/load_grammar.hpp"
#include "pmt/rt/ast.hpp"
#include "pmt/rt/ast_to_string.hpp"
#include "pmt/rt/pika_parser.hpp"
#include "pmt/util/read_file.hpp"

#include <fstream>
#include <iostream>

namespace {}  // namespace

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

 if (!args._input_test_file.empty()) {
  std::string const input_test = pmt::util::read_file(args._input_test_file);
  pmt::rt::MemoTable const memo_table = pmt::rt::PikaParser::populate_memo_table(input_test, pika_tables);
  pmt::rt::Ast::UniqueHandle ast_testfile = pmt::rt::PikaParser::memo_table_to_ast(memo_table, input_test, pika_tables);
  if (ast_testfile != nullptr) {
   pmt::rt::AstToString const ast_to_string([&](pmt::rt::IdType id_) { return pika_tables.get_id_table().id_to_string(id_); }, 2);
   std::cout << ast_to_string.to_string(*ast_testfile);
  } else {
   std::cout << "Failed to parse test input.\n";
  }
 }

 std::optional<std::ofstream> output_header;
 std::optional<std::ofstream> output_source;
 if (!args._output_header_file.empty()) {
  output_header.emplace(args._output_header_file);
 }
 if (!args._output_source_file.empty()) {
  output_source.emplace(args._output_source_file);
 }
 pmt::builder::PikaTablesEmitter pika_program_emitter(pmt::builder::PikaTablesEmitter::Args{
  ._program = pika_tables,
  ._header_include_path = args._header_include_filename,
  ._output_header = output_header ? &*output_header : nullptr,
  ._output_source = output_source ? &*output_source : nullptr,
  ._header_skel = output_header ? pmt::util::read_file(args._header_skel_file) : std::string(),
  ._source_skel = output_source ? pmt::util::read_file(args._source_skel_file) : std::string(),
  ._class_name = args._class_name,
  ._namespace_name = args._namespace_name,
 });
 pika_program_emitter.emit();

 std::optional<std::ofstream> output_id_strings;
 std::optional<std::ofstream> output_id_constants;
 if (!args._id_strings_output_file.empty()) {
  output_id_strings.emplace(args._id_strings_output_file);
 }
 if (!args._id_constants_output_file.empty()) {
  output_id_constants.emplace(args._id_constants_output_file);
 }
 pmt::builder::IdEmitter id_emitter(pmt::builder::IdEmitter::Args{
  ._id_table = pika_tables.get_id_table(),
  ._strings_output_file = output_id_strings ? &*output_id_strings : nullptr,
  ._strings_skel = output_id_strings ? pmt::util::read_file(args._id_strings_skel_file) : std::string(),
  ._constants_output_file = output_id_constants ? &*output_id_constants : nullptr,
  ._constants_skel = output_id_constants ? pmt::util::read_file(args._id_constants_skel_file) : std::string(),
 });
 id_emitter.emit();

 if (args._terminal_graph_output_file.has_value() && args._terminal_graph_skel_file.has_value()) {
  std::ofstream output_graph(*args._terminal_graph_output_file);
  pmt::builder::TerminalDotfileEmitter dot_writer(pmt::builder::TerminalDotfileEmitter::Args{
   ._final_id_to_string_fn = [&](pmt::rt::IdType idx_) { return std::to_string(idx_); },
   ._skel = pmt::util::read_file(*args._terminal_graph_skel_file),
   ._state_machine = pika_tables.get_terminal_state_machine_tables_full().get_state_machine(),
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
