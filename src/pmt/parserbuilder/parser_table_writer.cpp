#include "pmt/parserbuilder/parser_table_writer.hpp"

#include "pmt/base/bitset_converter.hpp"
#include "pmt/util/timestamp.hpp"
#include "pmt/util/smrt/util.hpp"
#include "pmt/parserbuilder/parser_tables.hpp"
#include "pmt/parserbuilder/table_writer_common.hpp"

#include <vector>

namespace pmt::parserbuilder {
 using namespace pmt::util::smrt;
 using namespace pmt::util::smct;
 using namespace pmt::base;

 namespace {

 }

 void ParserTableWriter::write(WriterArgs& writer_writer_args_) {
  _writer_args = &writer_writer_args_;

  _header = std::string(std::istreambuf_iterator<char>(_writer_args->_is_header_skel), std::istreambuf_iterator<char>());
  _source = std::string(std::istreambuf_iterator<char>(_writer_args->_is_source_skel), std::istreambuf_iterator<char>());

  replace_in_header();
  replace_in_source();

  _writer_args->_os_header << _header;
  _writer_args->_os_source << _source;
 }

 void ParserTableWriter::replace_in_header(){
  replace_timestamp(_header);
  replace_namespace_open(_header);
  replace_class_name(_header);
  replace_namespace_close(_header);
 }
 
 void ParserTableWriter::replace_in_source(){
  replace_timestamp(_source);
  replace_namespace_open(_source);
  replace_class_name(_source);
  replace_namespace_close(_source);
  replace_header_include_path(_source);
  replace_transitions(_source);
  replace_state_kinds(_source);
  replace_transition_masks(_source);
  replace_parser_accepts(_source);
  replace_parser_accept_count(_source);
  replace_parser_conflict_count(_source);
  replace_lookahead_accepts(_source);
  replace_parser_accepts_labels(_source);
  replace_parser_accepts_ids(_source);
  replace_parser_eoi_accept_index(_source);
  replace_id_names(_source);
  replace_min_id(_source);
  replace_id_count(_source);
  replace_parser_unpack(_source);
  replace_parser_hide(_source);
  replace_parser_merge(_source);
 }
 
 void ParserTableWriter::replace_namespace_open(std::string& str_) {
  if (_writer_args->_namespace_name.empty()) {
    return;
  }

  std::string const namespace_open_replacement = "namespace " + _writer_args->_namespace_name + " {";
  replace_skeleton_label(str_, "NAMESPACE_OPEN", namespace_open_replacement); 
 }
 
 void ParserTableWriter::replace_class_name(std::string& str_){
  replace_skeleton_label(str_, "CLASS_NAME", _writer_args->_class_name);
 }

 void ParserTableWriter::replace_namespace_close(std::string& str_){
  if (_writer_args->_namespace_name.empty()) {
    return;
  }

  std::string const namespace_close_replacement = "} // namespace " + _writer_args->_namespace_name + "\n";
  replace_skeleton_label(str_, "NAMESPACE_CLOSE", namespace_close_replacement);
 }
 
 void ParserTableWriter::replace_header_include_path(std::string& str_){
  replace_skeleton_label(str_, "HEADER_INCLUDE_PATH", _writer_args->_header_include_path);
 }

void ParserTableWriter::replace_transitions(std::string& str_) {
 TableWriterCommon::replace_transitions(*this, str_, "PARSER_", _writer_args->_tables.get_parser_state_machine());
 TableWriterCommon::replace_transitions(*this, str_, "LOOKAHEAD_", _writer_args->_tables.get_lookahead_state_machine());
}

void ParserTableWriter::replace_state_kinds(std::string& str_) {
  std::vector<size_t> state_kinds;
  state_kinds.reserve(_writer_args->_tables.get_parser_state_machine().get_state_nrs().size());
  _writer_args->_tables.get_parser_state_machine().get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      state_kinds.push_back(_writer_args->_tables.get_state_kind(state_nr_));
    }
  );

  TableWriterCommon::replace_array<size_t>(*this, str_, "PARSER_STATE_KINDS", state_kinds);
}

void ParserTableWriter::replace_transition_masks(std::string& str_) {
 TableWriterCommon::replace_transition_masks(*this, str_, "PARSER_TERMINAL_", _writer_args->_tables.get_parser_state_machine().get_state_count(), [&](pmt::util::smrt::StateNrType state_nr_) {return _writer_args->_tables.get_state_terminal_transitions(state_nr_);});
 TableWriterCommon::replace_transition_masks(*this, str_, "PARSER_HIDDEN_TERMINAL_", _writer_args->_tables.get_parser_state_machine().get_state_count(), [&](pmt::util::smrt::StateNrType state_nr_) {return _writer_args->_tables.get_state_hidden_terminal_transitions(state_nr_);});
 TableWriterCommon::replace_transition_masks(*this, str_, "PARSER_CONFLICT_", _writer_args->_tables.get_parser_state_machine().get_state_count(), [&](pmt::util::smrt::StateNrType state_nr_) {return _writer_args->_tables.get_state_conflict_transitions(state_nr_);});

 TableWriterCommon::replace_transition_masks(*this, str_, "LOOKAHEAD_TERMINAL_", _writer_args->_tables.get_lookahead_state_machine().get_state_count(), [&](pmt::util::smrt::StateNrType state_nr_) {return _writer_args->_tables.get_lookahead_state_terminal_transitions(state_nr_);});
}

void ParserTableWriter::replace_parser_accepts(std::string& str_){
 std::vector<size_t> accepts;

 _writer_args->_tables.get_parser_state_machine().get_state_nrs().for_each_key(
   [&](pmt::util::smrt::StateNrType state_nr_) {
    accepts.push_back(pmt::util::smrt::encode_accept_index(_writer_args->_tables.get_state_accept_index(state_nr_)));
   }
 );

 TableWriterCommon::replace_array<size_t>(*this, str_, "PARSER_ACCEPTS", accepts);
 replace_skeleton_label(str_, "PARSER_ACCEPT_CHUNK_COUNT", TableWriterCommon::as_hex(Bitset::get_required_chunk_count(_writer_args->_tables.get_accept_count()), true));
}

void ParserTableWriter::replace_parser_accept_count(std::string& str_) {
 std::string const parser_accept_count_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_accept_count(), true);
 replace_skeleton_label(str_, "PARSER_ACCEPT_COUNT", parser_accept_count_replacement);
}

void ParserTableWriter::replace_parser_conflict_count(std::string& str_) {
 replace_skeleton_label(str_, "PARSER_CONFLICT_COUNT", TableWriterCommon::as_hex(_writer_args->_tables.get_conflict_count(), true));
}

void ParserTableWriter::replace_lookahead_accepts(std::string& str_) {
  std::vector<Bitset::ChunkType> accepts_flattened;

  size_t accept_count = 0;
  _writer_args->_tables.get_lookahead_state_machine().get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      State const& state = *_writer_args->_tables.get_lookahead_state_machine().get_state(state_nr_);
      size_t const cur = state.get_accepts().popcnt() == 0 ? 0 : state.get_accepts().get_by_index(0).get_lower();
      accept_count = std::max(accept_count, cur);
    }
   );

  _writer_args->_tables.get_lookahead_state_machine().get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      State const& state = *_writer_args->_tables.get_lookahead_state_machine().get_state(state_nr_);
      Bitset accepts = BitsetConverter::from_interval_set(state.get_accepts());
      if (accepts.size() < accept_count) {
        accepts.resize(accept_count, false);
      }
      for (Bitset::ChunkType const& chunk : accepts.get_chunks()) {
        accepts_flattened.push_back(chunk);
      }
    }
   );
 
  TableWriterCommon::replace_array<Bitset::ChunkType>(*this, str_, "LOOKAHEAD_ACCEPTS", accepts_flattened);
  replace_skeleton_label(str_, "LOOKAHEAD_ACCEPTS_CHUNK_COUNT", TableWriterCommon::as_hex(Bitset::get_required_chunk_count(accept_count), true));
}

void ParserTableWriter::replace_parser_accepts_labels(std::string& str_){
  std::vector<std::string> accept_labels;
  accept_labels.reserve(_writer_args->_tables.get_accept_count());
  for (AcceptsIndexType i = 0; i < _writer_args->_tables.get_accept_count(); ++i) {
   accept_labels.push_back(_writer_args->_tables.get_accept_index_label(i));
  }

  TableWriterCommon::replace_array(*this, str_, "PARSER_ACCEPT_LABELS", accept_labels);
}

void ParserTableWriter::replace_parser_accepts_ids(std::string& str_) {
  std::vector<GenericId::IdType> accept_ids;
  accept_ids.reserve(_writer_args->_tables.get_accept_count());
  for (AcceptsIndexType i = 0; i < _writer_args->_tables.get_accept_count(); ++i) {
   accept_ids.push_back(_writer_args->_tables.get_accept_index_id(i));
  }

  TableWriterCommon::replace_array<GenericId::IdType>(*this, str_, "PARSER_ACCEPT_IDS", accept_ids);
}

void ParserTableWriter::replace_parser_eoi_accept_index(std::string& str_) {
 std::string const eoi_accept_index_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_eoi_accept_index(), true);
 replace_skeleton_label(str_, "EOI_ACCEPT_INDEX", eoi_accept_index_replacement);
}

void ParserTableWriter::replace_id_names(std::string& str_) {
 std::vector<std::string> id_names;
 id_names.reserve(_writer_args->_tables.get_id_count());

 for (GenericId::IdType i = _writer_args->_tables.get_min_id(); i < _writer_args->_tables.get_min_id() + _writer_args->_tables.get_id_count(); ++i) {
  id_names.push_back(_writer_args->_tables.id_to_string(i));
 }

 TableWriterCommon::replace_array(*this, str_, "ID_NAMES", id_names);
}

void ParserTableWriter::replace_min_id(std::string& str_) {
  std::string const min_id_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_min_id(), true);
  replace_skeleton_label(str_, "MIN_ID", min_id_replacement);
}

void ParserTableWriter::replace_id_count(std::string& str_) {
  std::string const id_count_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_id_count(), true);
  replace_skeleton_label(str_, "ID_COUNT", id_count_replacement);
}

void ParserTableWriter::replace_parser_unpack(std::string& str_) {
 Bitset unpack;
 for (AcceptsIndexType i = 0; i < _writer_args->_tables.get_accept_count(); ++i) {
  unpack.push_back(_writer_args->_tables.get_accept_index_unpack(i));
 }

 std::vector<Bitset::ChunkType> unpack_flattened;
 for (Bitset::ChunkType const& chunk : unpack.get_chunks()) {
  unpack_flattened.push_back(chunk);
 }

 TableWriterCommon::replace_array<Bitset::ChunkType>(*this, str_, "PARSER_UNPACK", unpack_flattened);
}

void ParserTableWriter::replace_parser_hide(std::string& str_) {
 Bitset hide;
 for (AcceptsIndexType i = 0; i < _writer_args->_tables.get_accept_count(); ++i) {
  hide.push_back(_writer_args->_tables.get_accept_index_hide(i));
 }

 std::vector<Bitset::ChunkType> hide_flattened;
 for (Bitset::ChunkType const& chunk : hide.get_chunks()) {
  hide_flattened.push_back(chunk);
 }

 TableWriterCommon::replace_array<Bitset::ChunkType>(*this, str_, "PARSER_HIDE", hide_flattened);
}

void ParserTableWriter::replace_parser_merge(std::string& str_) {
 Bitset merge;
 for (AcceptsIndexType i = 0; i < _writer_args->_tables.get_accept_count(); ++i) {
  merge.push_back(_writer_args->_tables.get_accept_index_merge(i));
 }

 std::vector<Bitset::ChunkType> merge_flattened;
 for (Bitset::ChunkType const& chunk : merge.get_chunks()) {
  merge_flattened.push_back(chunk);
 }

 TableWriterCommon::replace_array<Bitset::ChunkType>(*this, str_, "PARSER_MERGE", merge_flattened);
}

void ParserTableWriter::replace_timestamp(std::string& str_) {
  replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}

}