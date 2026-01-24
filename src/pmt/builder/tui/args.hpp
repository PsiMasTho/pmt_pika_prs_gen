#pragma once

#include <fstream>
#include <optional>
#include <string>

namespace pmt::builder::tui {

class Args {
public:
 Args(int argc_, char const* const* argv_);

 std::ifstream _input_grammar_file;
 std::optional<std::ifstream> _input_test_file;

 std::string _pika_program_header_include_filename;
 std::ofstream _pika_program_output_header_file;
 std::ofstream _pika_program_output_source_file;
 std::ifstream _pika_program_header_skel_file;
 std::ifstream _pika_program_source_skel_file;
 std::string _pika_program_class_name;
 std::string _pika_program_namespace_name;

 std::ofstream _id_strings_output_file;
 std::ifstream _id_strings_skel_file;

 std::ofstream _id_constants_output_file;
 std::ifstream _id_constants_skel_file;

 std::optional<std::ofstream> _output_grammar_file;
 std::optional<std::ofstream> _output_clauses_file;
 std::optional<std::ofstream> _terminal_graph_output_file;
 std::optional<std::ifstream> _terminal_graph_skel_file;
};

}  // namespace pmt::builder::tui
