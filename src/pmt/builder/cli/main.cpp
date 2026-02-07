#include "pmt/builder/cli/args.hpp"
#include "pmt/builder/id_emitter.hpp"
#include "pmt/builder/pika_tables.hpp"
#include "pmt/builder/pika_tables_emitter.hpp"
#include "pmt/builder/pika_tables_to_str.hpp"
#include "pmt/builder/terminal_dotfile_emitter.hpp"
#include "pmt/meta/grammar_loader.hpp"
#include "pmt/meta/grammar_to_str.hpp"
#include "pmt/rt/ast.hpp"
#include "pmt/rt/ast_to_string.hpp"
#include "pmt/rt/pika_parser.hpp"

#include <iostream>

namespace {}  // namespace

auto main(int argc_, char const* const* argv_) -> int try {
 pmt::builder::cli::Args args(argc_, argv_);

 pmt::meta::GrammarLoader grammar_loader;
 for (std::string const& input : args._input_grammar) {
  grammar_loader.push_input(input);
 }
 for (std::string const& start_rule : args._start_rules) {
  grammar_loader.push_start_rule(start_rule);
 }

 pmt::meta::Grammar const grammar = grammar_loader.load_grammar();

 if (args._output_grammar) {
  *args._output_grammar << pmt::meta::grammar_to_string(grammar);
 }

 pmt::builder::PikaTables const pika_tables(grammar);
 if (args._output_clauses) {
  *args._output_clauses << pmt::builder::pika_tables_to_string(pika_tables);
 }

 if (!args._input_test.empty()) {
  pmt::rt::MemoTable const memo_table = pmt::rt::PikaParser::populate_memo_table(args._input_test, pika_tables);
  pmt::rt::Ast::UniqueHandle ast_testfile = pmt::rt::PikaParser::memo_table_to_ast(memo_table, args._input_test, pika_tables);
  if (ast_testfile != nullptr) {
   pmt::rt::AstToString const ast_to_string([&](pmt::rt::IdType id_) { return pika_tables.get_id_table().id_to_string(id_); }, 2);
   std::cout << "Successfully parsed test input.\n";
   if (args._output_test) {
    *args._output_test << ast_to_string.to_string(*ast_testfile);
   }
  } else {
   std::cout << "Failed to parse test input.\n";
  }
 }

 pmt::builder::PikaTablesEmitter(pmt::builder::PikaTablesEmitter::Args{
                                  ._pika_tables = pika_tables,
                                  ._header_include_path = args._header_include_path,
                                  ._id_constants_include_path = args._id_constants_include_path,
                                  ._output_header = args._output_header.get(),
                                  ._output_source = args._output_source.get(),
                                  ._header_skel = args._header_skel,
                                  ._source_skel = args._source_skel,
                                  ._class_name = args._class_name,
                                  ._namespace_name = args._namespace_name,
                                 })
  .emit();

 pmt::builder::IdEmitter(pmt::builder::IdEmitter::Args{._id_table = pika_tables.get_id_table(), ._output_id_strings = args._output_id_strings.get(), ._id_strings_skel = args._id_strings_skel, ._output_id_constants = args._output_id_constants.get(), ._id_constants_skel = args._id_constants_skel}).emit();

 pmt::builder::TerminalDotfileEmitter(pmt::builder::TerminalDotfileEmitter::Args{._final_id_to_string_fn = [&](pmt::rt::IdType idx_) { return std::to_string(idx_); }, ._terminal_dotfile_skel = args._terminal_dotfile_skel, ._state_machine = pika_tables.get_terminal_state_machine_tables_full().get_state_machine(), ._output_terminal_dotfile = args._output_terminal_dotfile.get()}).write_dot();

} catch (std::exception const& e) {
 std::cerr << std::string(e.what()) << '\n';
 return 1;
} catch (...) {
 std::cerr << "Unhandled exception!\n";
 return 1;
}
