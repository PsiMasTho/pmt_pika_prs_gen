#include "pmt/parser/builder/id_constants_writer.hpp"

#include "pmt/parser/builder/parser_tables.hpp"
#include "pmt/parser/builder/lexer_tables.hpp"
#include "pmt/parser/builder/table_writer_common.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/util/timestamp.hpp"

namespace pmt::parser::builder {
using namespace pmt::util::sm;
using namespace pmt::base;

void IdConstantsWriter::write(WriterArgs& writer_args_) {
  _writer_args = &writer_args_;

  _id_constants = std::string(std::istreambuf_iterator<char>(_writer_args->_is_id_constants_skel), std::istreambuf_iterator<char>());

  replace_id_constants(_id_constants);
  replace_timestamp(_id_constants);

  _writer_args->_os_id_constants << _id_constants;
}

void IdConstantsWriter::replace_id_constants(std::string& str_) {
 std::string id_constants_replacement;

 for (GenericId::IdType i = _writer_args->_lexer_tables.get_min_id(); i < _writer_args->_lexer_tables.get_min_id() + _writer_args->_lexer_tables.get_id_count(); ++i) {
  id_constants_replacement += _writer_args->_lexer_tables.id_to_string(i) + " = " + TableWriterCommon::as_hex(i, true) + ",\n";
 }

 for (GenericId::IdType i = _writer_args->_parser_tables.get_min_id(); i < _writer_args->_parser_tables.get_min_id() + _writer_args->_parser_tables.get_id_count(); ++i) {
  id_constants_replacement += _writer_args->_parser_tables.id_to_string(i) + " = " + TableWriterCommon::as_hex(i, true) + ",\n";
 }

 replace_skeleton_label(str_, "ID_CONSTANTS", id_constants_replacement);
}

void IdConstantsWriter::replace_timestamp(std::string& str_) {
  replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}

}