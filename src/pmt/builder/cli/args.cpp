#include "pmt/builder/cli/args.hpp"

#include "argparse/argparse.hpp"
#include "pmt/util/skeleton_paths.hpp"

namespace pmt::builder::cli {

Args::Args(int argc_, char const* const* argv_) {
 argparse::ArgumentParser cmdl(argc_ > 0 ? argv_[0] : "pmt-builder");

 std::string skel_dir;
 cmdl.add_argument("--skel-dir").store_into(skel_dir);

 cmdl.add_argument("--input-grammar-file").required().store_into(_input_grammar_file);
 cmdl.add_argument("--input-test-file");

 cmdl.add_argument("--pika-program-header-include-filename").required().store_into(_pika_program_header_include_filename);
 cmdl.add_argument("--pika-program-output-header-file").required().store_into(_pika_program_output_header_file);
 cmdl.add_argument("--pika-program-output-source-file").required().store_into(_pika_program_output_source_file);
 cmdl.add_argument("--pika-program-header-skel-file");
 cmdl.add_argument("--pika-program-source-skel-file");
 cmdl.add_argument("--pika-program-class-name");
 cmdl.add_argument("--pika-program-namespace-name");

 cmdl.add_argument("--id-strings-output-file").required().store_into(_id_strings_output_file);
 cmdl.add_argument("--id-strings-skel-file");
 cmdl.add_argument("--id-constants-output-file").required().store_into(_id_constants_output_file);
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

 _pika_program_header_skel_file = cmdl.present<std::string>("pika-program-header-skel-file").value_or(pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/pika_program-skel.hpp"));
 _pika_program_source_skel_file = cmdl.present<std::string>("pika-program-source-skel-file").value_or(pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/pika_program-skel.cpp"));
 _pika_program_class_name = cmdl.present<std::string>("pika-program-class-name").value_or(std::string());
 _pika_program_namespace_name = cmdl.present<std::string>("pika-program-namespace-name").value_or(std::string());

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
