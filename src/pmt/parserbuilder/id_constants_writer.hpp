#pragma once

#include "pmt/fw_decl.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <ostream>

PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, LexerTables)
PMT_FW_DECL_NS_CLASS(pmt::parserbuilder, ParserTables)

namespace pmt::parserbuilder {

class IdConstantsWriter : public pmt::util::SkeletonReplacerBase {
 public:
 // -$ Types / Constants $-
 struct WriterArgs {
  std::ostream& _os_id_constants;
  std::istream& _is_id_constants_skel;
  LexerTables const& _lexer_tables;
  ParserTables const& _parser_tables;
 };

 private:
 // -$ Data $-
 WriterArgs* _writer_args = nullptr;
 std::string _id_constants;

 public:
  // -$ Functions $-
  // --$ Other $--
  void write(WriterArgs& writer_args_);

  private:
  void replace_id_constants(std::string& str_);

  void replace_timestamp(std::string& str_);
};

}