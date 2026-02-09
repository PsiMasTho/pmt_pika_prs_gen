#include "pmt/builder/cli/args.hpp"

#include "argparse/argparse.hpp"

#include "pmt/builder/skeleton_paths.hpp"
#include "pmt/util/read_file.hpp"

namespace pmt::builder::cli {
namespace {
void setup_argument_parser(argparse::ArgumentParser& cmdl_) {
 cmdl_.add_argument("--start-rules").help("Space seperated list of one or more grammar start rule names without the prefix $. Only one start rule name is allowed for table generation").nargs(argparse::nargs_pattern::at_least_one).required();

 cmdl_.add_argument("--input-grammar").help("Space seperated list of one or more grammar files").nargs(argparse::nargs_pattern::at_least_one).required();
 cmdl_.add_argument("--input-test").help("Test file for generated parser");

 cmdl_.add_argument("--output-header").help("Output location of the generated parsing table header");
 cmdl_.add_argument("--output-source").help("Output location of the generated parsing table source");
 cmdl_.add_argument("--output-test").help("Output location of the printed AST from parsing the test file");
 cmdl_.add_argument("--output-id-strings").help("Output location of the generated id strings");
 cmdl_.add_argument("--output-id-constants").help("Output location of the generated id constants");
 cmdl_.add_argument("--output-grammar").help("Output location of the processed grammar (debug information)");
 cmdl_.add_argument("--output-clauses").help("Output location of the generated clause list (debug information)");
 cmdl_.add_argument("--output-terminal-dotfile").help("Output location of the generated terminal state machine dotfile (debug information)");

 cmdl_.add_argument("--header-include-path").help("The string that will be emitted in an #include directive inside the output source file to find the output header");
 cmdl_.add_argument("--id-constants-include-path").help("The string that will be emitted in an #include directive inside the output source file to find the id constants");
 cmdl_.add_argument("--class-name").help("The name of the generated parsing table class");
 cmdl_.add_argument("--namespace-name").help("The enclosing namespace of the generated parsing table class");

 cmdl_.add_argument("--skel-dir").help("Location of .skel file directory. Skel files are found relative to this directory unless they are individually specified");
 cmdl_.add_argument("--id-strings-skel").help("Location of the id strings skel file");
 cmdl_.add_argument("--id-constants-skel").help("Location of the id constants skel file");
 cmdl_.add_argument("--terminal-dotfile-skel").help("Location of the terminal dotfile skel file");
 cmdl_.add_argument("--header-skel").help("Location of the parsing table header skel file");
 cmdl_.add_argument("--source-skel").help("Location of the parsing table source skel file");
}

void assign_from_args(std::string_view arg_name_, std::vector<std::string>& dest_, argparse::ArgumentParser const& cmdl_) {
 dest_ = cmdl_.present<std::vector<std::string>>(arg_name_).value_or(std::vector<std::string>{});
}

void assign_from_arg(std::string_view arg_name_, std::string& dest_, argparse::ArgumentParser const& cmdl_, std::string_view default_value_ = "") {
 dest_ = cmdl_.present<std::string>(arg_name_).value_or(std::string(default_value_));
}

void assign_optional_from_arg(std::string_view arg_name_, std::optional<std::string>& dest_, argparse::ArgumentParser const& cmdl_) {
 dest_ = cmdl_.present(arg_name_);
}

void read_optional_file_from_arg(std::string_view arg_name_, std::optional<std::string>& dest_, argparse::ArgumentParser const& cmdl_) {
 std::optional<std::string> path;
 assign_optional_from_arg(arg_name_, path, cmdl_);
 if (path.has_value()) {
  dest_ = pmt::util::read_file(*path);
 } else {
  dest_.reset();
 }
}

void read_files_from_arg(std::string_view arg_name_, std::vector<std::string>& dest_, argparse::ArgumentParser const& cmdl_) {
 std::vector<std::string> paths;
 assign_from_args(arg_name_, paths, cmdl_);
 dest_.clear();
 dest_.reserve(paths.size());
 for (std::string const& path : paths) {
  if (path.empty()) {
   continue;
  }
  dest_.push_back(pmt::util::read_file(path));
 }
}

void handle_start_rules_args(Args& args_, argparse::ArgumentParser const& cmdl_) {
 assign_from_args("--start-rules", args_._start_rules, cmdl_);
}

void handle_input_args(Args& args_, argparse::ArgumentParser& cmdl_) {
 read_files_from_arg("--input-grammar", args_._input_grammar, cmdl_);
 read_optional_file_from_arg("--input-test", args_._input_test, cmdl_);
}

void handle_output_args(Args& args_, argparse::ArgumentParser& cmdl_) {
 assign_optional_from_arg("--output-header", args_._output_header_path, cmdl_);
 assign_optional_from_arg("--output-source", args_._output_source_path, cmdl_);

 if (args_._input_test.has_value()) {
  assign_optional_from_arg("--output-test", args_._output_test_path, cmdl_);
 }

 assign_optional_from_arg("--output-id-strings", args_._output_id_strings_path, cmdl_);
 assign_optional_from_arg("--output-id-constants", args_._output_id_constants_path, cmdl_);
 assign_optional_from_arg("--output-grammar", args_._output_grammar_path, cmdl_);
 assign_optional_from_arg("--output-clauses", args_._output_clauses_path, cmdl_);
 assign_optional_from_arg("--output-terminal-dotfile", args_._output_terminal_dotfile_path, cmdl_);
}

void handle_substitution_args(Args& args_, argparse::ArgumentParser& cmdl_) {
 assign_from_arg("--header-include-path", args_._header_include_path, cmdl_);
 assign_from_arg("--id-constants-include-path", args_._id_constants_include_path, cmdl_);
 assign_from_arg("--class-name", args_._class_name, cmdl_, "PikaTables");
 assign_from_arg("--namespace-name", args_._namespace_name, cmdl_);
}

void handle_skel_args(Args& args_, argparse::ArgumentParser& cmdl_) {
 SkeletonPaths skeleton_paths;
 if (std::string skel_dir = cmdl_.present("--skel-dir").value_or(""); !skel_dir.empty()) {
  skeleton_paths.set_root_override(skel_dir);
 }
 if (args_._output_id_strings_path.has_value()) {
  args_._id_strings_skel = pmt::util::read_file(cmdl_.present("--id-strings-skel").value_or(skeleton_paths.resolve("cpp/id_strings-skel.hpp")));
 }
 if (args_._output_id_constants_path.has_value()) {
  args_._id_constants_skel = pmt::util::read_file(cmdl_.present("--id-constants-skel").value_or(skeleton_paths.resolve("cpp/id_constants-skel.hpp")));
 }
 if (args_._output_terminal_dotfile_path.has_value()) {
  args_._terminal_dotfile_skel = pmt::util::read_file(cmdl_.present("--terminal-dotfile-skel").value_or(skeleton_paths.resolve("dot/state_machine-skel.dot")));
 }
 if (args_._output_header_path.has_value()) {
  args_._header_skel = pmt::util::read_file(cmdl_.present("--header-skel").value_or(skeleton_paths.resolve("cpp/pika_tables-skel.hpp")));
 }
 if (args_._output_source_path.has_value()) {
  args_._source_skel = pmt::util::read_file(cmdl_.present("--source-skel").value_or(skeleton_paths.resolve("cpp/pika_tables-skel.cpp")));
 }
}

void check_multiple_start_rule_validity(Args const& args_) {
 // If there is more than one start rule, we can't parse or generate parsing tables
 if (args_._start_rules.size() == 1) {
  return;  // No restrictions
 }

 if (args_._output_source_path.has_value()) {
  throw std::runtime_error("Only exactly one start rule allowed with --output-source");
 }
 if (!args_._input_test.has_value()) {
  throw std::runtime_error("Only exactly one start rule allowed with --input-test");
 }
}

}  // namespace

Args::Args(int argc_, char const* const* argv_) {
 argparse::ArgumentParser cmdl("pmt_pika_prs_gen_cli", "1.0", argparse::default_arguments::help);
 setup_argument_parser(cmdl);
 cmdl.parse_args(argc_, argv_);

 // The ordering of these matters!
 handle_start_rules_args(*this, cmdl);
 handle_input_args(*this, cmdl);
 handle_output_args(*this, cmdl);
 handle_substitution_args(*this, cmdl);
 handle_skel_args(*this, cmdl);

 check_multiple_start_rule_validity(*this);
}

}  // namespace pmt::builder::cli
