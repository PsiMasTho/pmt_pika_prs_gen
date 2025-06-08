#include "pmt/parser/builder/lexer_table_writer.hpp"

#include "pmt/base/bitset_converter.hpp"
#include "pmt/util/timestamp.hpp"
#include "pmt/parser/builder/lexer_tables.hpp"
#include "pmt/parser/builder/table_writer_common.hpp"

#include <set>
#include <vector>

namespace pmt::parser::builder {
 using namespace pmt::util::sm::ct;
 using namespace pmt::util::sm;
 using namespace pmt::base;

 namespace {
 }

 void LexerTableWriter::write(WriterArgs& writer_args_) {
  _writer_args = &writer_args_;

  _header = std::string(std::istreambuf_iterator<char>(_writer_args->_is_header_skel), std::istreambuf_iterator<char>());
  _source = std::string(std::istreambuf_iterator<char>(_writer_args->_is_source_skel), std::istreambuf_iterator<char>());

  replace_in_header();
  replace_in_source();

  _writer_args->_os_header << _header;
  _writer_args->_os_source << _source;
 }

 void LexerTableWriter::replace_in_header(){
  replace_timestamp(_header);
  replace_namespace_open(_header);
  replace_class_name(_header);
  replace_namespace_close(_header);
 }
 
 void LexerTableWriter::replace_in_source(){
  replace_timestamp(_source);
  replace_namespace_open(_source);
  replace_class_name(_source);
  replace_namespace_close(_source);
  replace_header_include_path(_source);
  replace_transitions(_source);
  replace_accepts(_source);
  replace_accept_count(_source);
  replace_accept_labels(_source);
  replace_accept_ids(_source);
  replace_id_names(_source);
  replace_min_id(_source);
  replace_id_count(_source);
  replace_linecount_accepts(_source);
  replace_terminal_count(_source);
  replace_start_terminal_index(_source);
  replace_eoi_terminal_index(_source);
 }
 
 void LexerTableWriter::replace_namespace_open(std::string& str_) {
  if (_writer_args->_namespace_name.empty()) {
    return;
  }

  std::string const namespace_open_replacement = "namespace " + _writer_args->_namespace_name + " {";
  replace_skeleton_label(str_, "NAMESPACE_OPEN", namespace_open_replacement); 
 }
 
 void LexerTableWriter::replace_class_name(std::string& str_){
  replace_skeleton_label(str_, "CLASS_NAME", _writer_args->_class_name);
 }

 void LexerTableWriter::replace_namespace_close(std::string& str_){
  if (_writer_args->_namespace_name.empty()) {
    return;
  }

  std::string const namespace_close_replacement = "} // namespace " + _writer_args->_namespace_name + "\n";
  replace_skeleton_label(str_, "NAMESPACE_CLOSE", namespace_close_replacement);
 }
 
 void LexerTableWriter::replace_header_include_path(std::string& str_){
  replace_skeleton_label(str_, "HEADER_INCLUDE_PATH", _writer_args->_header_include_path);
 }
 
 void LexerTableWriter::replace_transitions(std::string& str_) {
  TableWriterCommon::replace_transitions(*this, str_, "LEXER_", _writer_args->_tables.get_lexer_state_machine());
  TableWriterCommon::replace_transitions(*this, str_, "LINECOUNT_", _writer_args->_tables.get_linecount_state_machine());
 }
 
 void LexerTableWriter::replace_accepts(std::string& str_){
  std::vector<Bitset::ChunkType> accepts_flattened;

  size_t const accept_count = _writer_args->_tables.get_accept_count();

  _writer_args->_tables.get_lexer_state_machine().get_state_nrs().for_each_key(
    [&](StateNrType state_nr_) {
      State const& state = *_writer_args->_tables.get_lexer_state_machine().get_state(state_nr_);
      Bitset terminals = BitsetConverter::from_interval_set(state.get_accepts());
      if (terminals.size() < accept_count) {
        terminals.resize(accept_count, false);
      }
      for (Bitset::ChunkType const& chunk : terminals.get_chunks()) {
        accepts_flattened.push_back(chunk);
      }
    }
   );
 
  TableWriterCommon::replace_array<Bitset::ChunkType>(*this, str_, "LEXER_ACCEPTS", accepts_flattened);
  replace_skeleton_label(str_, "LEXER_ACCEPT_CHUNK_COUNT", TableWriterCommon::as_hex(Bitset::get_required_chunk_count(accept_count), true));
 }

void LexerTableWriter::replace_accept_count(std::string& str_) {
 replace_skeleton_label(str_, "LEXER_ACCEPT_COUNT", TableWriterCommon::as_hex(_writer_args->_tables.get_accept_count(), true));
}
 
 void LexerTableWriter::replace_accept_labels(std::string& str_){
  std::vector<std::string> accept_labels;
  accept_labels.reserve(_writer_args->_tables.get_accept_count());
  for (AcceptsIndexType i = 0; i < _writer_args->_tables.get_accept_count(); ++i) {
   accept_labels.push_back(_writer_args->_tables.get_accept_index_label(i));
  }

  TableWriterCommon::replace_array(*this, str_, "LEXER_ACCEPT_LABELS", accept_labels);
 }
 
 void LexerTableWriter::replace_accept_ids(std::string& str_){
  std::vector<GenericId::IdType> accept_ids;
  accept_ids.reserve(_writer_args->_tables.get_accept_count());
  for (AcceptsIndexType i = 0; i < _writer_args->_tables.get_accept_count(); ++i) {
   accept_ids.push_back(_writer_args->_tables.get_accept_index_id(i));
  }

  TableWriterCommon::replace_array<GenericId::IdType>(*this, str_, "LEXER_ACCEPT_IDS", accept_ids);
 }
 
void LexerTableWriter::replace_id_names(std::string& str_){
 std::vector<std::string> id_names;
 id_names.reserve(_writer_args->_tables.get_id_count());

 for (GenericId::IdType i = _writer_args->_tables.get_min_id(); i < _writer_args->_tables.get_min_id() + _writer_args->_tables.get_id_count(); ++i) {
  id_names.push_back(_writer_args->_tables.id_to_string(i));
 }

 TableWriterCommon::replace_array(*this, str_, "ID_NAMES", id_names);
}

void LexerTableWriter::replace_min_id(std::string& str_) {
 std::string const min_id_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_min_id(), true);
 replace_skeleton_label(str_, "MIN_ID", min_id_replacement);
}

void LexerTableWriter::replace_id_count(std::string& str_) {
  std::string const id_count_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_id_count(), true);
  replace_skeleton_label(str_, "ID_COUNT", id_count_replacement);
}

 void LexerTableWriter::replace_linecount_accepts(std::string& str_) {
  std::set<StateNrType> linecount_accepts;
  _writer_args->_tables.get_linecount_state_machine().get_state_nrs().for_each_key(
    [&](StateNrType state_nr_) {
      if (!_writer_args->_tables.get_linecount_state_machine().get_state(state_nr_)->get_accepts().empty()) {
        linecount_accepts.insert(state_nr_);
      }
    }
   );

   TableWriterCommon::replace_array(*this, str_, "LINECOUNT_ACCEPTS", linecount_accepts.begin(), linecount_accepts.end());
 }
 
void LexerTableWriter::replace_terminal_count(std::string& str_){
 size_t const accept_count = _writer_args->_tables.get_accept_count();
 replace_skeleton_label(str_, "ACCEPT_COUNT", TableWriterCommon::as_hex(accept_count, true));
 replace_skeleton_label(str_, "ACCEPT_CHUNK_COUNT", TableWriterCommon::as_hex(Bitset::get_required_chunk_count(accept_count), true));
}
 
void LexerTableWriter::replace_start_terminal_index(std::string& str_){
 std::string start_terminal_index_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_start_accept_index(), true);
 replace_skeleton_label(str_, "START_ACCEPT_INDEX", start_terminal_index_replacement);
}
 
void LexerTableWriter::replace_eoi_terminal_index(std::string& str_){
 std::string eoi_terminal_index_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_eoi_accept_index(), true);
 replace_skeleton_label(str_, "EOI_ACCEPT_INDEX", eoi_terminal_index_replacement);
}

void LexerTableWriter::replace_timestamp(std::string& str_) {
  replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}
}