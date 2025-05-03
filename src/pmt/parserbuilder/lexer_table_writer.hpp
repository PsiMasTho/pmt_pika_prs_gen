#pragma once

#include "pmt/fw_decl.hpp"

#include <ostream>

PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, LexerTables)

namespace pmt::parserbuilder {

class LexerTableWriter {
 public:
 // -$ Types / Constants $-
 enum class IncludeQuotes {
  Angle,
  DoubleQuote
 };

 struct Arguments {
  std::string _namespace_name; // No namespace if empty
  std::string _class_name;
  std::string _header_include_path; // For the source file to include the header file
  std::string _header_include_guard_define; // #pragma once if empty
  size_t _indent_increment = 2; // Number of spaces to indent
  IncludeQuotes _pmt_lib_include_quotes = IncludeQuotes::Angle; // Include quotes for pmt lib includes
  IncludeQuotes _generated_header_include_quotes = IncludeQuotes::DoubleQuote; // Include quotes for generated header include
 };

 private:
 // -$ Data $-
 std::ostream& _os_header;
 std::ostream& _os_source;
 std::ostream& _os_id_constants;
 LexerTables const& _tables;
 Arguments const* _args = nullptr;
 size_t _indent = 0;

 public:
 // -$ Functions $-
 // --$ Lifetime $--
  LexerTableWriter(std::ostream& os_header_, std::ostream& os_source_, std::ostream& os_id_constants_, LexerTables const& tables_);

  // --$ Other $--
  void write(Arguments const& args_);

  private:
  void write_header();
  void write_source();
  void write_id_constants();

  void write_header_includes();

  void write_anonymous_namespace_top(std::ostream& os_);
  void write_anonymous_namespace_bottom(std::ostream& os_);

  void write_namespace_top(std::ostream& os_);
  void write_namespace_bottom(std::ostream& os_);

  void write_header_class_top();
  void write_header_class_body();
  void write_header_class_bottom();
  void write_header_include_guard_top();
  void write_header_include_guard_bottom();

  void write_source_includes();
  void write_source_transitions();
  void write_source_accepts();
  void write_source_id_names();
  void write_source_function_definitions();

  void write_source_get_state_nr_next();
  void write_source_get_state_accepts();
  void write_source_get_accept_count();
  void write_source_get_start_accept_index();
  void write_source_get_eoi_accept_index();
  void write_source_get_accept_string();
  void write_source_get_accept_id();
  void write_source_id_to_string();

  auto get_indent() -> std::string;
  void increment_indent();
  void decrement_indent();

  void write_include_statement(std::ostream& os_, std::string const& include_path_, IncludeQuotes quotes_);

  static auto get_open_quote(IncludeQuotes quotes_) -> char;
  static auto get_close_quote(IncludeQuotes quotes_) -> char;
};

}