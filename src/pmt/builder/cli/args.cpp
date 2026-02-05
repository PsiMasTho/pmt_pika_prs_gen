#include "pmt/builder/cli/args.hpp"

#include "argparse/argparse.hpp"
#include "pmt/util/skeleton_paths.hpp"

#include <string_view>
namespace pmt::builder::cli {

Args::Args(int argc_, char const* const* argv_) {
 bool test_mode = false;
 for (int i = 1; i < argc_; ++i) {
  std::string_view const arg = argv_[i];
  if (arg == "--input-test-file" || arg.starts_with("--input-test-file=")) {
   test_mode = true;
   break;
  }
 }

 argparse::ArgumentParser cmdl("pmt_pika_prs_gen_cli", "1.0", argparse::default_arguments::help);

 std::string skel_dir;
 cmdl.add_argument("--skel-dir").store_into(skel_dir);

 cmdl.add_argument("--input-grammar-file").required().store_into(_input_grammar_file);
 cmdl.add_argument("--input-test-file");

 auto& pika_tables_header_include_arg = cmdl.add_argument("--pika-tables-header-include-filename");
 if (!test_mode) {
  pika_tables_header_include_arg.required();
 }
 pika_tables_header_include_arg.store_into(_pika_tables_header_include_filename);

 auto& pika_tables_output_header_arg = cmdl.add_argument("--pika-tables-output-header-file");
 if (!test_mode) {
  pika_tables_output_header_arg.required();
 }
 pika_tables_output_header_arg.store_into(_pika_tables_output_header_file);

 auto& pika_tables_output_source_arg = cmdl.add_argument("--pika-tables-output-source-file");
 if (!test_mode) {
  pika_tables_output_source_arg.required();
 }
 pika_tables_output_source_arg.store_into(_pika_tables_output_source_file);
 cmdl.add_argument("--pika-tables-header-skel-file");
 cmdl.add_argument("--pika-tables-source-skel-file");
 cmdl.add_argument("--pika-tables-class-name");
 cmdl.add_argument("--pika-tables-namespace-name");

 auto& id_strings_output_arg = cmdl.add_argument("--id-strings-output-file");
 if (!test_mode) {
  id_strings_output_arg.required();
 }
 id_strings_output_arg.store_into(_id_strings_output_file);
 cmdl.add_argument("--id-strings-skel-file");
 auto& id_constants_output_arg = cmdl.add_argument("--id-constants-output-file");
 if (!test_mode) {
  id_constants_output_arg.required();
 }
 id_constants_output_arg.store_into(_id_constants_output_file);
 cmdl.add_argument("--id-constants-skel-file");

 cmdl.add_argument("--output-grammar");
 cmdl.add_argument("--output-clauses");
 cmdl.add_argument("--output-dotfile");
 cmdl.add_argument("--terminal-graph-skel-file");

 cmdl.parse_args(argc_, argv_);

 if (!skel_dir.empty()) {
  pmt::util::SkeletonPathsSingleton::instance()->set_root_override(std::move(skel_dir));
 }

 _input_test_file = cmdl.present<std::string>("input-test-file");
 _test_mode = test_mode;

 if (_test_mode) {
  auto provided = [&](std::string_view name_) {
   return cmdl.present<std::string>(name_).has_value();
  };
  std::vector<std::string_view> forbidden;
  if (provided("skel-dir"))
   forbidden.push_back("--skel-dir");
  if (provided("pika-tables-header-include-filename"))
   forbidden.push_back("--pika-tables-header-include-filename");
  if (provided("pika-tables-output-header-file"))
   forbidden.push_back("--pika-tables-output-header-file");
  if (provided("pika-tables-output-source-file"))
   forbidden.push_back("--pika-tables-output-source-file");
  if (provided("pika-tables-header-skel-file"))
   forbidden.push_back("--pika-tables-header-skel-file");
  if (provided("pika-tables-source-skel-file"))
   forbidden.push_back("--pika-tables-source-skel-file");
  if (provided("pika-tables-class-name"))
   forbidden.push_back("--pika-tables-class-name");
  if (provided("pika-tables-namespace-name"))
   forbidden.push_back("--pika-tables-namespace-name");
  if (provided("id-strings-output-file"))
   forbidden.push_back("--id-strings-output-file");
  if (provided("id-strings-skel-file"))
   forbidden.push_back("--id-strings-skel-file");
  if (provided("id-constants-output-file"))
   forbidden.push_back("--id-constants-output-file");
  if (provided("id-constants-skel-file"))
   forbidden.push_back("--id-constants-skel-file");
  if (provided("terminal-graph-skel-file"))
   forbidden.push_back("--terminal-graph-skel-file");

  if (!forbidden.empty()) {
   std::string msg = "Test mode does not allow: ";
   std::string delim;
   for (std::string_view const arg : forbidden) {
    msg += std::exchange(delim, ", ") + std::string(arg);
   }
   throw std::runtime_error(msg);
  }
 }

 _pika_tables_header_skel_file = cmdl.present<std::string>("pika-tables-header-skel-file").value_or(pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/pika_tables-skel.hpp"));
 _pika_tables_source_skel_file = cmdl.present<std::string>("pika-tables-source-skel-file").value_or(pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/pika_tables-skel.cpp"));
 _pika_tables_class_name = cmdl.present<std::string>("pika-tables-class-name").value_or("PikaTables");
 _pika_tables_namespace_name = cmdl.present<std::string>("pika-tables-namespace-name").value_or(std::string());

 _id_strings_skel_file = cmdl.present<std::string>("id-strings-skel-file").value_or(pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/id_strings-skel.hpp"));
 _id_constants_skel_file = cmdl.present<std::string>("id-constants-skel-file").value_or(pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/id_constants-skel.hpp"));

 _output_grammar_file = cmdl.present<std::string>("output-grammar");
 _output_clauses_file = cmdl.present<std::string>("output-clauses");
 _terminal_graph_output_file = cmdl.present<std::string>("output-dotfile");

 if (_terminal_graph_output_file.has_value()) {
  _terminal_graph_skel_file = cmdl.present<std::string>("terminal-graph-skel-file").value_or(pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/state_machine-skel.dot"));
 }
}

}  // namespace pmt::builder::cli
