#pragma once

#include "pmt/base/bitset.hpp"
#include "pmt/fw_decl.hpp"

#include <ostream>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericLexerTables);

namespace pmt::parserbuilder {

class TableWriter {
 public:
  TableWriter(std::ostream& os_header_, std::ostream& os_source_, std::string header_path_, std::string namespace_name_, std::string class_name_, pmt::util::smrt::GenericLexerTables const& tables_);

  void write();

 private:
  void write_header();
  void write_source();

  void write_header_id_enum();
  void write_source_id_to_string_defintion();
  void write_source_as_generic_lexer_tables_definition();

  template <typename T_>
  void write_pair_entries(std::vector<T_> const& entries_, std::string const& label_);

  template <typename T_>
  void write_single_entries(std::vector<T_> const& entries_, std::string const& label_);

  void write_single_entries(std::vector<std::string> const& entries_, std::string const& label_);

  void write_single_entries(std::vector<pmt::base::Bitset> const& entries_, std::string const& label_);

  static auto as_hex(std::integral auto value_, bool hex_prefix_ = true) -> std::string;

  static inline size_t const PER_LINE = 16;
  static inline size_t const PER_LINE_HEX_PAIRS = 6;

  std::ostream& _os_header;
  std::ostream& _os_source;
  pmt::util::smrt::GenericLexerTables const& _tables;
  std::string _header_path;
  std::string _namespace_name;
  std::string _class_name;
};

}  // namespace pmt::parserbuilder

#include "pmt/parserbuilder/table_writer-inl.hpp"