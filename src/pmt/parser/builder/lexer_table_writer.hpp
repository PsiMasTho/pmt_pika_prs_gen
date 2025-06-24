#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <ostream>

PMT_FW_DECL_NS_CLASS(pmt::parser::builder, LexerTables)

namespace pmt::parser::builder {

class LexerTableWriter : public pmt::util::SkeletonReplacerBase {
public:
 // -$ Types / Constants $-
 struct WriterArgs {
  std::ostream& _os_header;
  std::ostream& _os_source;
  std::istream& _is_header_skel;
  std::istream& _is_source_skel;
  LexerTables const& _tables;
  std::string _namespace_name;  // No namespace if empty
  std::string _class_name;
  std::string _header_include_path;
 };

private:
 // -$ Data $-
 WriterArgs* _writer_args = nullptr;
 std::string _header;
 std::string _source;

public:
 // -$ Functions $-
 // --$ Other $--
 void write(WriterArgs& writer_args_);

private:
 void replace_in_header();
 void replace_in_source();

 void replace_namespace_open(std::string& str_);
 void replace_class_name(std::string& str_);
 void replace_namespace_close(std::string& str_);

 void replace_header_include_path(std::string& str_);
 void replace_transitions(std::string& str_);
 void replace_accepts(std::string& str_);
 void replace_accept_count(std::string& str_);
 void replace_accept_display_names(std::string& str_);
 void replace_accept_ids(std::string& str_);
 void replace_linecount_accepts(std::string& str_);
 void replace_terminal_count(std::string& str_);
 void replace_start_terminal_index(std::string& str_);
 void replace_eoi_terminal_index(std::string& str_);

 void replace_timestamp(std::string& str_);
};

}  // namespace pmt::parser::builder