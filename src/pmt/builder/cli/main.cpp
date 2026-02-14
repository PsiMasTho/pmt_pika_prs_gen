#include "pmt/builder/cli/args.hpp"
#include "pmt/builder/id_table_to_cpp_str.hpp"
#include "pmt/builder/pika_tables.hpp"
#include "pmt/builder/pika_tables_to_cpp_str.hpp"
#include "pmt/builder/pika_tables_to_txt_str.hpp"
#include "pmt/meta/grammar_loader.hpp"
#include "pmt/meta/grammar_to_pika_str.hpp"
#include "pmt/rt/ast.hpp"
#include "pmt/rt/ast_to_string.hpp"
#include "pmt/rt/pika_parser.hpp"
#include "pmt/sm/sm_to_dot_str.hpp"
#include "pmt/util/singleton.hpp"
#include "pmt/util/skeleton_replacer.hpp"

#include <fstream>
#include <iostream>

namespace {}  // namespace

auto main(int argc_, char const* const* argv_) -> int try {
 auto const skeleton_replacer = pmt::util::Singleton<pmt::util::SkeletonReplacer>::instance();

 pmt::builder::cli::Args const args(argc_, argv_);

 pmt::meta::GrammarLoader grammar_loader;
 for (std::string const& input : args._input_grammar) {
  grammar_loader.push_input(input);
 }
 for (std::string const& start_rule : args._start_rules) {
  grammar_loader.push_start_rule(start_rule);
 }

 pmt::meta::Grammar const grammar = grammar_loader.load_grammar();

 if (args._output_grammar_path.has_value()) {
  std::ofstream(*args._output_grammar_path) << pmt::meta::grammar_to_pika_str(grammar);
 }

 pmt::builder::PikaTables const pika_tables(grammar);
 if (args._output_clauses_path.has_value()) {
  std::ofstream(*args._output_clauses_path) << pmt::builder::pika_tables_to_txt_str(pika_tables);
 }

 if (args._input_test.has_value()) {
  pmt::rt::MemoTable const memo_table = pmt::rt::PikaParser::populate_memo_table(*args._input_test, pika_tables);
  pmt::rt::Ast::UniqueHandle ast_testfile = pmt::rt::PikaParser::memo_table_to_ast(memo_table, *args._input_test, pika_tables);
  if (ast_testfile != nullptr) {
   pmt::rt::AstToString const ast_to_string([&](pmt::rt::IdType id_) { return pika_tables.get_id_table().id_to_string(id_); }, 2);
   std::cout << "Successfully parsed test input.\n";
   if (args._output_test_path.has_value()) {
    std::ofstream(*args._output_test_path) << ast_to_string.to_string(*ast_testfile);
   }
  } else {
   std::cout << "Failed to parse test input.\n";
  }
 }

 if (args._output_header_path.has_value()) {
  std::ofstream(*args._output_header_path) << pmt::builder::pika_tables_to_cpp_str_hdr(args._class_name, args._namespace_name, args._header_skel);
 }

 if (args._output_source_path.has_value()) {
  std::ofstream(*args._output_source_path) << pmt::builder::pika_tables_to_cpp_str_src(pika_tables, args._class_name, args._namespace_name, args._header_include_path, args._id_constants_include_path, args._source_skel);
 }

 if (args._output_id_constants_path.has_value()) {
  std::ofstream(*args._output_id_constants_path) << pmt::builder::id_table_constants_to_cpp_str(pika_tables.get_id_table(), args._id_constants_skel);
 }

 if (args._output_id_strings_path.has_value()) {
  std::ofstream(*args._output_id_strings_path) << pmt::builder::id_table_strings_to_cpp_str(pika_tables.get_id_table(), args._id_strings_skel);
 }

 if (args._output_terminal_dotfile_path.has_value()) {
  std::ofstream(*args._output_terminal_dotfile_path) << pmt::sm::sm_to_dot_str(
   pika_tables.get_terminal_state_machine_tables_full().get_state_machine(), [&](pmt::sm::FinalIdType id_) { return std::to_string(id_); }, args._terminal_dotfile_skel);
 }
} catch (std::exception const& e) {
 std::cerr << std::string(e.what()) << '\n';
 return 1;
} catch (...) {
 std::cerr << "Unhandled exception!\n";
 return 1;
}
