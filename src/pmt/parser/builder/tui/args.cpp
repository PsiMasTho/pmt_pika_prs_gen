#include "pmt/parser/builder/tui/args.hpp"

#include <stdexcept>

#include "argh/argh.h"

namespace pmt::parser::builder::tui {

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

 try_open_file_from_arg(cmdl, "input-grammar-file", _input_grammar_file);
 try_open_optional_file_from_arg(cmdl, "input-test-file", _input_test_file);
 try_fetch_required_arg(cmdl, "output-lexer-header-file", _lexer_header_include_filename);
 try_open_file_from_arg(cmdl, "output-lexer-header-file", _output_lexer_header_file);
 try_open_file_from_arg(cmdl, "output-lexer-source-file", _output_lexer_source_file);
 try_open_required_file_from_optional_arg(cmdl, "lexer-header-skel-file", _lexer_header_skel_file, "/home/pmt/repos/pmt/skel/pmt/parser/builder/lexer_tables-skel.hpp");
 try_open_required_file_from_optional_arg(cmdl, "lexer-source-skel-file", _lexer_source_skel_file, "/home/pmt/repos/pmt/skel/pmt/parser/builder/lexer_tables-skel.cpp");
 try_fetch_required_arg(cmdl, "output-parser-header-file", _parser_header_include_filename);
 try_open_file_from_arg(cmdl, "output-parser-header-file", _output_parser_header_file);
 try_open_file_from_arg(cmdl, "output-parser-source-file", _output_parser_source_file);
 try_open_required_file_from_optional_arg(cmdl, "parser-header-skel-file", _parser_header_skel_file, "/home/pmt/repos/pmt/skel/pmt/parser/builder/parser_tables-skel.hpp");
 try_open_required_file_from_optional_arg(cmdl, "parser-source-skel-file", _parser_source_skel_file, "/home/pmt/repos/pmt/skel/pmt/parser/builder/parser_tables-skel.cpp");
 try_open_file_from_arg(cmdl, "output-id-constants-file", _output_id_constants_file);
 try_open_required_file_from_optional_arg(cmdl, "id-constants-skel-file", _id_constants_skel_file, "/home/pmt/repos/pmt/skel/pmt/parser/builder/id_constants-skel.hpp");
 try_open_file_from_arg(cmdl, "output-id-strings-file", _output_id_strings_file);
 try_open_required_file_from_optional_arg(cmdl, "id-strings-skel-file", _id_strings_skel_file, "/home/pmt/repos/pmt/skel/pmt/parser/builder/id_strings-skel.hpp");
 try_fetch_required_arg(cmdl, "lexer-class-name", _lexer_class_name);
 try_fetch_required_arg(cmdl, "parser-class-name", _parser_class_name);
 try_fetch_required_arg(cmdl, "namespace-name", _namespace_name);

 _write_dotfiles = !cmdl["no-dotfiles"];
}

}  // namespace pmt::parser::builder::tui