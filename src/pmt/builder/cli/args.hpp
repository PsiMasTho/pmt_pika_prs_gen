#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace pmt::builder::cli {

class Args {
public:
 // -$ Data $-
 std::vector<std::string> _start_rules;

 std::vector<std::string> _input_grammar;
 std::string _input_test;

 std::unique_ptr<std::ostream> _output_header;
 std::unique_ptr<std::ostream> _output_source;
 std::unique_ptr<std::ostream> _output_test;
 std::unique_ptr<std::ostream> _output_id_strings;
 std::unique_ptr<std::ostream> _output_id_constants;
 std::unique_ptr<std::ostream> _output_grammar;
 std::unique_ptr<std::ostream> _output_clauses;
 std::unique_ptr<std::ostream> _output_terminal_dotfile;

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
