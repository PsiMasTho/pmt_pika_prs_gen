#pragma once

#include <optional>
#include <string>

namespace pmt::builder::cli {

class Args {
public:
 Args(int argc_, char const* const* argv_);

 std::string _input_grammar_file;
 std::optional<std::string> _input_test_file;
 bool _test_mode = false;

 std::string _pika_tables_header_include_filename;
 std::string _pika_tables_output_header_file;
 std::string _pika_tables_output_source_file;
 std::string _pika_tables_header_skel_file;
 std::string _pika_tables_source_skel_file;
 std::string _pika_tables_class_name;
 std::string _pika_tables_namespace_name;

 std::string _id_strings_output_file;
 std::string _id_strings_skel_file;

 std::string _id_constants_output_file;
 std::string _id_constants_skel_file;

 std::optional<std::string> _output_grammar_file;
 std::optional<std::string> _output_clauses_file;
 std::optional<std::string> _terminal_graph_output_file;
 std::optional<std::string> _terminal_graph_skel_file;
};

}  // namespace pmt::builder::cli
