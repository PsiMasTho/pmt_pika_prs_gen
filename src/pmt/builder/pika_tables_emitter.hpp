#pragma once

#include "pmt/builder/pika_tables.hpp"
#include "pmt/fw_decl.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <iosfwd>
#include <span>
#include <string>

PMT_FW_DECL_NS_CLASS(pmt::container, Bitset);

namespace pmt::builder {

class PikaTablesEmitter : public pmt::util::SkeletonReplacerBase {
public:
 // -$ Types / Constants $-
 class Args {
 public:
  PikaTables const& _pika_tables;
  std::string _header_include_path;
  std::string _id_constants_include_path;
  std::ostream* _output_header = nullptr;
  std::ostream* _output_source = nullptr;
  std::string _header_skel;
  std::string _source_skel;
  std::string _class_name;
  std::string _namespace_name;
  size_t _ch_per_line_max = 120;
 };

private:
 // -$ Data $-
 Args _args;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 explicit PikaTablesEmitter(Args args_);

 // --$ Other $--
 void emit();

private:
 void replace_terminal_tables();
 void replace_clauses();
 void replace_rules();
 void replace_numeric_list(std::string& dest_, std::string_view label_typename_, std::string_view label_list_, std::span<uint64_t const> values_, bool padded_digits_);
 void replace_tag_list(std::string& dest_, std::string_view label_list_, std::span<pmt::rt::ClauseBase::Tag const> values_);
 void replace_numeric_enum_list(std::string& dest_, std::string_view label_list_, std::span<std::string const> values_);
 void replace_string_list(std::string& dest_, std::string_view label_list_, std::span<std::string const> values_, bool add_quotes_);
 void replace_number(std::string& dest_, std::string_view label_typename_, std::string_view label_value_, uint64_t value_);
 void replace_bitset(std::string& dest_, std::string_view label_typename_, std::string_view label_chunk_count_, std::string_view label_value_, pmt::container::Bitset const& value_);
};

}  // namespace pmt::builder
