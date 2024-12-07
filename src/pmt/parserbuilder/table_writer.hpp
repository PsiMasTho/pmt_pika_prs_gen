#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/parse/generic_lexer_tables.hpp"

#include <iosfwd>

PMT_FW_DECL_NS_CLASS(pmt::util::parse, GenericLexerTables);

namespace pmt::parserbuilder {

class TableWriter {
 public:
  TableWriter(std::ostream& os_header_, std::ostream& os_source_, std::string_view class_name_, pmt::util::parse::GenericLexerTables const& tables_);

  void write();

 private:
  void write_header();
  void write_source();

  std::ostream& _os_header;
  std::ostream& _os_source;
  std::string_view _namespace_name;
  std::string_view _class_name;
  pmt::util::parse::GenericLexerTables const& _tables;
};

}  // namespace pmt::parserbuilder