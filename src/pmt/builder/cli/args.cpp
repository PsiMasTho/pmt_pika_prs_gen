#include "pmt/builder/cli/args.hpp"

#include "argparse/argparse.hpp"
#include "pmt/builder/skeleton_paths.hpp"

namespace pmt::builder::cli {

Args::Args(int argc_, char const* const* argv_) {
 SkeletonPaths skeleton_paths;

 argparse::ArgumentParser cmdl("pmt_pika_prs_gen_cli", "1.0", argparse::default_arguments::help);

 std::string skel_dir;
 cmdl.add_argument("--skel-dir").store_into(skel_dir);

 cmdl.add_argument("--input-grammar-file").required().store_into(_input_grammar_file);
 cmdl.add_argument("--input-test-file");

 auto& pika_tables_header_include_arg = cmdl.add_argument("--header-include-filename");
 pika_tables_header_include_arg.store_into(_header_include_filename);

 auto& pika_tables_output_header_arg = cmdl.add_argument("--output-header-file");
 pika_tables_output_header_arg.store_into(_output_header_file);

 auto& pika_tables_output_source_arg = cmdl.add_argument("--output-source-file");
 pika_tables_output_source_arg.store_into(_output_source_file);
 cmdl.add_argument("--header-skel-file");
 cmdl.add_argument("--source-skel-file");
 cmdl.add_argument("--class-name");
 cmdl.add_argument("--namespace-name");

 auto& id_strings_output_arg = cmdl.add_argument("--id-strings-output-file");
 id_strings_output_arg.store_into(_id_strings_output_file);
 cmdl.add_argument("--id-strings-skel-file");
 auto& id_constants_output_arg = cmdl.add_argument("--id-constants-output-file");
 id_constants_output_arg.store_into(_id_constants_output_file);
 cmdl.add_argument("--id-constants-skel-file");

 cmdl.add_argument("--output-grammar");
 cmdl.add_argument("--output-clauses");
 cmdl.add_argument("--output-dotfile");
 cmdl.add_argument("--terminal-graph-skel-file");

 cmdl.parse_args(argc_, argv_);

 if (!skel_dir.empty()) {
  skeleton_paths.set_root_override(std::move(skel_dir));
 }

 _input_test_file = cmdl.present<std::string>("input-test-file").value_or("");

 _header_skel_file = cmdl.present<std::string>("header-skel-file").value_or("");
 _source_skel_file = cmdl.present<std::string>("source-skel-file").value_or("");
 _class_name = cmdl.present<std::string>("class-name").value_or("PikaTables");
 _namespace_name = cmdl.present<std::string>("namespace-name").value_or("");

 _id_strings_skel_file = cmdl.present<std::string>("id-strings-skel-file").value_or("");
 _id_constants_skel_file = cmdl.present<std::string>("id-constants-skel-file").value_or("");

 _output_grammar_file = cmdl.present<std::string>("output-grammar");
 _output_clauses_file = cmdl.present<std::string>("output-clauses");
 _terminal_graph_output_file = cmdl.present<std::string>("output-dotfile");

 if (!_output_header_file.empty() && _header_skel_file.empty()) {
  _header_skel_file = skeleton_paths.get_path("cpp/pika_tables-skel.hpp");
 }
 if (!_output_source_file.empty() && _source_skel_file.empty()) {
  _source_skel_file = skeleton_paths.get_path("cpp/pika_tables-skel.cpp");
 }
 if (!_id_strings_output_file.empty() && _id_strings_skel_file.empty()) {
  _id_strings_skel_file = skeleton_paths.get_path("cpp/id_strings-skel.hpp");
 }
 if (!_id_constants_output_file.empty() && _id_constants_skel_file.empty()) {
  _id_constants_skel_file = skeleton_paths.get_path("cpp/id_constants-skel.hpp");
 }

 if (_terminal_graph_output_file.has_value()) {
  _terminal_graph_skel_file = cmdl.present<std::string>("terminal-graph-skel-file").value_or(skeleton_paths.get_path("dot/state_machine-skel.dot"));
 }
}

}  // namespace pmt::builder::cli
