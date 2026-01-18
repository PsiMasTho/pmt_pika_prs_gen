#include "pmt/builder/tui/args.hpp"

#include <stdexcept>

#include "argh/argh.h"
#include "pmt/util/skeleton_paths.hpp"

namespace pmt::builder::tui {

namespace {
void try_fetch_required_arg(argh::parser& cmdl_, std::string const& arg_name_, auto& out_value_) {
 if (!(cmdl_(arg_name_) >> out_value_)) {
  throw std::runtime_error("Missing required argument: --" + arg_name_);
 }
}

void try_open_file_from_arg(argh::parser& cmdl_, std::string const& arg_name_, auto& file_stream_) {
 std::string file_path;
 try_fetch_required_arg(cmdl_, arg_name_, file_path);
 file_stream_.open(file_path);
 if (!file_stream_.is_open()) {
  throw std::runtime_error("Failed to open file: " + file_path);
 }
}

void try_fetch_optional_arg(argh::parser& cmdl_, std::string const& arg_name_, auto& out_value_) {
 cmdl_(arg_name_) >> out_value_;
}

void try_open_optional_file_from_arg(argh::parser& cmdl_, std::string const& arg_name_, auto& optional_file_stream_) {
 std::string file_path;
 try_fetch_optional_arg(cmdl_, arg_name_, file_path);
 if (!file_path.empty()) {
  optional_file_stream_.emplace(file_path);
  if (!optional_file_stream_->is_open()) {
   throw std::runtime_error("Failed to open file: " + file_path);
  }
 } else {
  optional_file_stream_.reset();
 }
}

void try_fetch_optional_arg(argh::parser& cmdl_, std::string const& arg_name_, auto& out_value_, auto const& default_value_) {
 if (!(cmdl_(arg_name_) >> out_value_)) {
  out_value_ = default_value_;
 }
}

void try_open_required_file_from_optional_arg(argh::parser& cmdl_, std::string const& arg_name_, auto& file_stream_, std::string const& default_file_path_) {
 std::string file_path;
 try_fetch_optional_arg(cmdl_, arg_name_, file_path, default_file_path_);
 if (!file_path.empty()) {
  file_stream_.open(file_path);
  if (!file_stream_.is_open()) {
   throw std::runtime_error("Failed to open file: " + file_path);
  }
 }
}
}  // namespace

Args::Args(int argc_, char const* const* argv_) {
 argh::parser cmdl;
 cmdl.parse(argc_, argv_, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

 std::string skel_dir;
 try_fetch_optional_arg(cmdl, "skel-dir", skel_dir);
 if (!skel_dir.empty()) {
  pmt::util::SkeletonPathsSingleton::instance()->set_root_override(std::move(skel_dir));
 }

 try_open_file_from_arg(cmdl, "input-grammar-file", _input_grammar_file);
 try_open_optional_file_from_arg(cmdl, "input-test-file", _input_test_file);
 try_fetch_required_arg(cmdl, "pika-program-header-include-filename", _pika_program_header_include_filename);
 try_open_file_from_arg(cmdl, "pika-program-output-header-file", _pika_program_output_header_file);
 try_open_file_from_arg(cmdl, "pika-program-output-source-file", _pika_program_output_source_file);
 try_open_required_file_from_optional_arg(
  cmdl,
  "pika-program-header-skel-file",
  _pika_program_header_skel_file,
  pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/pika_program-skel.hpp"));
 try_open_required_file_from_optional_arg(
  cmdl,
  "pika-program-source-skel-file",
  _pika_program_source_skel_file,
  pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/pika_program-skel.cpp"));
 try_fetch_optional_arg(cmdl, "pika-program-class-name", _pika_program_class_name, std::string());
 try_fetch_optional_arg(cmdl, "pika-program-namespace-name", _pika_program_namespace_name, std::string());

 try_open_file_from_arg(cmdl, "id-strings-output-file", _id_strings_output_file);
 try_open_required_file_from_optional_arg(
  cmdl,
  "id-strings-skel-file",
  _id_strings_skel_file,
  pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/id_strings-skel.hpp"));
 try_open_file_from_arg(cmdl, "id-constants-output-file", _id_constants_output_file);
 try_open_required_file_from_optional_arg(
  cmdl,
  "id-constants-skel-file",
  _id_constants_skel_file,
  pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/id_constants-skel.hpp"));

 _write_dotfiles = !cmdl["no-dotfiles"];

 if (_write_dotfiles) {
  try_open_required_file_from_optional_arg(cmdl, "terminal-graph-output-file", _terminal_graph_output_file, "terminal_graph.dot");
  try_open_required_file_from_optional_arg(
   cmdl,
   "terminal-graph-skel-file",
   _terminal_graph_skel_file,
   pmt::util::SkeletonPathsSingleton::instance()->get_path("pmt/builder/state_machine-skel.dot"));
 }
}

}  // namespace pmt::builder::tui
