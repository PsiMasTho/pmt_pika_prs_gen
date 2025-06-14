#include "pmt/parser/builder/id_constants_writer.hpp"

#include "pmt/parser/builder/table_writer_common.hpp"
#include "pmt/parser/grammar/grammar_data.hpp"

namespace pmt::parser::builder {
using namespace pmt::util::sm;
using namespace pmt::base;

void IdConstantsWriter::write(WriterArgs& writer_args_) {
  _writer_args = &writer_args_;

  _id_constants = std::string(std::istreambuf_iterator<char>(_writer_args->_is_id_constants_skel), std::istreambuf_iterator<char>());

  replace_id_constants(_id_constants);
  TableWriterCommon::replace_timestamp(*this, _id_constants);

  _writer_args->_os_id_constants << _id_constants;
}

void IdConstantsWriter::replace_id_constants(std::string& str_) {
  std::string id_constants_replacement;

  for (auto const& [id_string, id_value] : _writer_args->_grammar_data.get_non_generic_ids()) {
    id_constants_replacement += id_string + " = " + TableWriterCommon::as_hex(id_value, true) + ",\n";
  }

  replace_skeleton_label(str_, "ID_CONSTANTS", id_constants_replacement);
}

}  // namespace pmt::parser::builder