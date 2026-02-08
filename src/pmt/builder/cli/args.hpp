#pragma once

#include <optional>
#include <string>
#include <vector>

namespace pmt::builder::cli {

class Args {
public:
 // -$ Data $-
 std::vector<std::string> _start_rules;

 std::vector<std::string> _input_grammar;
 std::optional<std::string> _input_test;

 std::optional<std::string> _output_header_path;
 std::optional<std::string> _output_source_path;
 std::optional<std::string> _output_test_path;
 std::optional<std::string> _output_id_strings_path;
 std::optional<std::string> _output_id_constants_path;
 std::optional<std::string> _output_grammar_path;
 std::optional<std::string> _output_clauses_path;
 std::optional<std::string> _output_terminal_dotfile_path;

 std::string _header_include_path;
 std::string _id_constants_include_path;
 std::string _class_name;
 std::string _namespace_name;

 std::string _id_strings_skel;
 std::string _id_constants_skel;
 std::string _terminal_dotfile_skel;
 std::string _header_skel;
 std::string _source_skel;

 // -$ Functions $-
 // --$ Lifetime $--
 Args(int argc_, char const* const* argv_);
};

}  // namespace pmt::builder::cli
