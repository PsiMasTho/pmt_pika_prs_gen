#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"

#include <iosfwd>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, LexerTables);

namespace pmt::parserbuilder {

class TableWriter {
 public:
  TableWriter(std::ostream& os_header_, std::ostream& os_source_, std::string_view class_name_, LexerTables const& tables_);

  void write();

 private:
  void write_header();
  void write_source();

  void write_header_id_enum();

  void write_line(auto&& first_, auto&& rest_...);

  std::ostream& _os_header;
  std::ostream& _os_source;
  LexerTables const& _tables;
  std::string_view _namespace_name;
  std::string_view _class_name;
  size_t _indent = 0;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/table_writer-inl.hpp"