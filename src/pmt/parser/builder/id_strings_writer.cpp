#include "pmt/parser/builder/id_strings_writer.hpp"

#include "pmt/parser/builder/table_writer_common.hpp"
#include "pmt/parser/grammar/grammar_data.hpp"

namespace pmt::parser::builder {
using namespace pmt::util::sm;
using namespace pmt::base;

void IdStringsWriter::write(WriterArgs& writer_args_) {
  _writer_args = &writer_args_;

  _id_strings = std::string(std::istreambuf_iterator<char>(_writer_args->_is_id_strings_skel), std::istreambuf_iterator<char>());

  replace_id_strings(_id_strings);
  TableWriterCommon::replace_timestamp(*this, _id_strings);

  _writer_args->_os_id_strings << _id_strings;
}

void IdStringsWriter::replace_id_strings(std::string& str_) {
  std::string id_constants_replacement;

  for (auto const& [id_string, _] : _writer_args->_grammar_data.get_non_generic_ids()) {
    id_constants_replacement += "\"" + id_string + "\",\n";
  }

  replace_skeleton_label(str_, "ID_STRINGS", id_constants_replacement);
}

}  // namespace pmt::parser::builder