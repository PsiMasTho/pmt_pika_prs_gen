#include "pmt/parserbuilder/parser_table_writer.hpp"

#include "pmt/util/timestamp.hpp"
#include "pmt/parserbuilder/parser_tables.hpp"
#include "pmt/parserbuilder/table_writer_common.hpp"

#include <vector>

namespace pmt::parserbuilder {
 using namespace pmt::util::smrt;
 using namespace pmt::util::smct;
 using namespace pmt::base;

 namespace {

 auto encode_parser_accept_index(size_t index_) -> size_t {
  return index_ + 1;
 }

 }

 void ParserTableWriter::write(WriterArgs& writer_writer_args_) {
  _writer_args = &writer_writer_args_;

  _header = std::string(std::istreambuf_iterator<char>(_writer_args->_is_header_skel), std::istreambuf_iterator<char>());
  _source = std::string(std::istreambuf_iterator<char>(_writer_args->_is_source_skel), std::istreambuf_iterator<char>());
  _id_constants = std::string(std::istreambuf_iterator<char>(_writer_args->_is_id_constants_skel), std::istreambuf_iterator<char>());

  replace_in_header();
  replace_in_source();
  replace_in_id_constants();

  _writer_args->_os_header << _header;
  _writer_args->_os_source << _source;
  _writer_args->_os_id_constants << _id_constants;
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
  replace_parser_transitions(_source);
  replace_state_kinds(_source);
  replace_parser_terminal_transition_masks(_source);
  replace_parser_conflict_transition_masks(_source);
  replace_lookahead_terminal_transition_masks(_source);
  replace_lookahead_transitions(_source);
  replace_parser_accepts(_source);
  replace_parser_accept_count(_source);
  replace_lookahead_accepts(_source);
  replace_parser_accepts_labels(_source);
  replace_parser_accepts_ids(_source);
  replace_id_names(_source);
  replace_min_id(_source);
  replace_id_count(_source);
  replace_parser_unpack(_source);
  replace_parser_hide(_source);
  replace_parser_merge(_source);
 }
 
 void ParserTableWriter::replace_in_id_constants(){
  replace_timestamp(_id_constants);
  replace_id_constants(_id_constants);
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

void ParserTableWriter::replace_parser_transitions(std::string& str_) {
  std::vector<SymbolType> lower_bounds;
  std::vector<SymbolType> upper_bounds;
  std::vector<StateNrType> values;
  std::vector<size_t> offsets{0};
 
   _writer_args->_tables._parser_state_machine.get_state_nrs().for_each_key(
     [&](pmt::util::smrt::StateNrType state_nr_) {
       State const& state = *_writer_args->_tables._parser_state_machine.get_state(state_nr_);
       IntervalMap<SymbolType, StateNrType> const& symbol_transitions = state.get_symbol_transitions();
       symbol_transitions.for_each_interval([&](StateNrType value_, Interval<SymbolType> interval_){
           lower_bounds.push_back(interval_.get_lower());
           upper_bounds.push_back(interval_.get_upper());
           values.push_back(value_);
       });
       offsets.push_back(offsets.back() + symbol_transitions.size());
     }
   );

   {
    std::stringstream lower_bounds_replacement;
    TableWriterCommon::write_single_entries<SymbolType>(lower_bounds_replacement, lower_bounds);
    replace_skeleton_label(str_, "PARSER_LOWER_BOUNDS", lower_bounds_replacement.str());
   }
   std::string const lower_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(lower_bounds);
   replace_skeleton_label(str_, "PARSER_LOWER_BOUNDS_TYPE", lower_bounds_type_replacement);
   
   std::stringstream upper_bounds_replacement;
   {
    TableWriterCommon::write_single_entries<SymbolType>(upper_bounds_replacement, upper_bounds);
    replace_skeleton_label(str_, "PARSER_UPPER_BOUNDS", upper_bounds_replacement.str());
   }
   std::string const upper_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(upper_bounds);
   replace_skeleton_label(str_, "PARSER_UPPER_BOUNDS_TYPE", upper_bounds_type_replacement);

   std::stringstream values_replacement;
   {
    TableWriterCommon::write_single_entries<StateNrType>(values_replacement, values);
    replace_skeleton_label(str_, "PARSER_VALUES", values_replacement.str());
   }
   std::string const values_type_replacement = TableWriterCommon::get_smallest_unsigned_type<StateNrType>(values);
   replace_skeleton_label(str_, "PARSER_VALUES_TYPE", values_type_replacement);

   std::stringstream offsets_replacement;
   {
    TableWriterCommon::write_single_entries<size_t>(offsets_replacement, offsets);
    replace_skeleton_label(str_, "PARSER_OFFSETS", offsets_replacement.str());
   }
   std::string const offsets_type_replacement = TableWriterCommon::get_smallest_unsigned_type<size_t>(offsets);
   replace_skeleton_label(str_, "PARSER_OFFSETS_TYPE", offsets_type_replacement);
}

void ParserTableWriter::replace_state_kinds(std::string& str_) {
  std::vector<size_t> state_kinds;
  state_kinds.reserve(_writer_args->_tables._parser_state_machine.get_state_nrs().size());
  _writer_args->_tables._parser_state_machine.get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      state_kinds.push_back(_writer_args->_tables.get_state_kind(state_nr_));
    }
  );

  std::stringstream state_kinds_replacement;
  TableWriterCommon::write_single_entries<size_t>(state_kinds_replacement, state_kinds);
  replace_skeleton_label(str_, "STATE_KINDS", state_kinds_replacement.str());
  std::string const state_kinds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<size_t>(state_kinds);
  replace_skeleton_label(str_, "STATE_KINDS_TYPE", state_kinds_type_replacement);
}

void ParserTableWriter::replace_parser_terminal_transition_masks(std::string& str_) {
 std::vector<Bitset::ChunkType> terminal_transition_masks_flattened;
 size_t const chunk_count = Bitset::get_required_chunk_count(_writer_args->_tables._parser_terminal_transition_masks.begin()->second.size());
 _writer_args->_tables._parser_state_machine.get_state_nrs().for_each_key(
   [&](pmt::util::smrt::StateNrType state_nr_) {
    Bitset::ChunkSpanConst const chunks = _writer_args->_tables.get_state_terminal_transitions(state_nr_);
    for (size_t i = 0; i < chunk_count; ++i) {
      terminal_transition_masks_flattened.push_back((i < chunks.size()) ? chunks[i] : Bitset::ALL_SET_MASKS[false]);
    }
   });

 std::stringstream terminal_transition_masks_replacement;
 TableWriterCommon::write_single_entries<Bitset::ChunkType>(terminal_transition_masks_replacement, terminal_transition_masks_flattened);
 replace_skeleton_label(str_, "PARSER_TERMINAL_TRANSITION_MASKS", terminal_transition_masks_replacement.str());
 replace_skeleton_label(str_, "PARSER_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT", TableWriterCommon::as_hex(chunk_count, true));
}

void ParserTableWriter::replace_parser_conflict_transition_masks(std::string& str_) {
 std::vector<Bitset::ChunkType> conflict_transition_masks_flattened;
 size_t const chunk_count = _writer_args->_tables._lookahead_terminal_transition_masks.empty() ? 0 : Bitset::get_required_chunk_count(_writer_args->_tables._parser_conflict_transition_masks.begin()->second.size());
 _writer_args->_tables._parser_state_machine.get_state_nrs().for_each_key(
   [&](pmt::util::smrt::StateNrType state_nr_) {
    Bitset::ChunkSpanConst const chunks = _writer_args->_tables.get_state_conflict_transitions(state_nr_);
    for (size_t i = 0; i < chunk_count; ++i) {
      conflict_transition_masks_flattened.push_back((i < chunks.size()) ? chunks[i] : Bitset::ALL_SET_MASKS[false]);
    }
   });

 std::stringstream conflict_transition_masks_replacement;
 TableWriterCommon::write_single_entries<Bitset::ChunkType>(conflict_transition_masks_replacement, conflict_transition_masks_flattened);
 replace_skeleton_label(str_, "PARSER_CONFLICT_TRANSITION_MASKS", conflict_transition_masks_replacement.str());
 replace_skeleton_label(str_, "PARSER_CONFLICT_TRANSITION_MASKS_CHUNK_COUNT", TableWriterCommon::as_hex(chunk_count, true));
 replace_skeleton_label(str_, "PARSER_CONFLICT_COUNT", TableWriterCommon::as_hex(_writer_args->_tables.get_conflict_count(), true));
}

void ParserTableWriter::replace_lookahead_terminal_transition_masks(std::string& str_) {
 std::vector<Bitset::ChunkType> terminal_transition_masks_flattened;
 size_t const chunk_count = _writer_args->_tables._lookahead_terminal_transition_masks.empty() ? 0 : Bitset::get_required_chunk_count(_writer_args->_tables._lookahead_terminal_transition_masks.begin()->second.size());
 _writer_args->_tables._lookahead_state_machine.get_state_nrs().for_each_key(
   [&](pmt::util::smrt::StateNrType state_nr_) {
    Bitset::ChunkSpanConst const chunks = _writer_args->_tables.get_lookahead_state_terminal_transitions(state_nr_);
    for (size_t i = 0; i < chunk_count; ++i) {
      terminal_transition_masks_flattened.push_back((i < chunks.size()) ? chunks[i] : Bitset::ALL_SET_MASKS[false]);
    }
   });

 std::stringstream terminal_transition_masks_replacement;
 TableWriterCommon::write_single_entries<Bitset::ChunkType>(terminal_transition_masks_replacement, terminal_transition_masks_flattened);
 replace_skeleton_label(str_, "LOOKAHEAD_TERMINAL_TRANSITION_MASKS", terminal_transition_masks_replacement.str());
 replace_skeleton_label(str_, "LOOKAHEAD_TERMINAL_TRANSITION_MASKS_CHUNK_COUNT", TableWriterCommon::as_hex(chunk_count, true));
}

void ParserTableWriter::replace_lookahead_transitions(std::string& str_) {
  std::vector<SymbolType> lower_bounds;
  std::vector<SymbolType> upper_bounds;
  std::vector<StateNrType> values;
  std::vector<size_t> offsets{0};
 
   _writer_args->_tables._lookahead_state_machine.get_state_nrs().for_each_key(
     [&](pmt::util::smrt::StateNrType state_nr_) {
       State const& state = *_writer_args->_tables._lookahead_state_machine.get_state(state_nr_);
       IntervalMap<SymbolType, StateNrType> const& symbol_transitions = state.get_symbol_transitions();
       symbol_transitions.for_each_interval([&](StateNrType value_, Interval<SymbolType> interval_){
           lower_bounds.push_back(interval_.get_lower());
           upper_bounds.push_back(interval_.get_upper());
           values.push_back(value_);
       });
       offsets.push_back(offsets.back() + symbol_transitions.size());
     }
   );

   {
    std::stringstream lower_bounds_replacement;
    TableWriterCommon::write_single_entries<SymbolType>(lower_bounds_replacement, lower_bounds);
    replace_skeleton_label(str_, "LOOKAHEAD_LOWER_BOUNDS", lower_bounds_replacement.str());
   }
   std::string const lower_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(lower_bounds);
   replace_skeleton_label(str_, "LOOKAHEAD_LOWER_BOUNDS_TYPE", lower_bounds_type_replacement);
   
   std::stringstream upper_bounds_replacement;
   {
    TableWriterCommon::write_single_entries<SymbolType>(upper_bounds_replacement, upper_bounds);
    replace_skeleton_label(str_, "LOOKAHEAD_UPPER_BOUNDS", upper_bounds_replacement.str());
   }
   std::string const upper_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(upper_bounds);
   replace_skeleton_label(str_, "LOOKAHEAD_UPPER_BOUNDS_TYPE", upper_bounds_type_replacement);

   std::stringstream values_replacement;
   {
    TableWriterCommon::write_single_entries<StateNrType>(values_replacement, values);
    replace_skeleton_label(str_, "LOOKAHEAD_VALUES", values_replacement.str());
   }
   std::string const values_type_replacement = TableWriterCommon::get_smallest_unsigned_type<StateNrType>(values);
   replace_skeleton_label(str_, "LOOKAHEAD_VALUES_TYPE", values_type_replacement);

   std::stringstream offsets_replacement;
   {
    TableWriterCommon::write_single_entries<size_t>(offsets_replacement, offsets);
    replace_skeleton_label(str_, "LOOKAHEAD_OFFSETS", offsets_replacement.str());
   }
   std::string const offsets_type_replacement = TableWriterCommon::get_smallest_unsigned_type<size_t>(offsets);
   replace_skeleton_label(str_, "LOOKAHEAD_OFFSETS_TYPE", offsets_type_replacement);
}

void ParserTableWriter::replace_parser_accepts(std::string& str_){
 std::vector<size_t> accepts;

 _writer_args->_tables._parser_state_machine.get_state_nrs().for_each_key(
   [&](pmt::util::smrt::StateNrType state_nr_) {
    accepts.push_back(encode_parser_accept_index(_writer_args->_tables.get_state_accept_index(state_nr_)));
   }
 );

 std::stringstream accepts_replacement;
 TableWriterCommon::write_single_entries<size_t>(accepts_replacement, accepts);
 replace_skeleton_label(str_, "PARSER_ACCEPTS", accepts_replacement.str());

 std::string const accepts_type_replacement = TableWriterCommon::get_smallest_unsigned_type<size_t>(accepts);
 replace_skeleton_label(str_, "PARSER_ACCEPTS_TYPE", accepts_type_replacement);
}

void ParserTableWriter::replace_parser_accept_count(std::string& str_) {
 std::string const parser_accept_count_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_accept_count(), true);
 replace_skeleton_label(str_, "PARSER_ACCEPT_COUNT", parser_accept_count_replacement);
}

void ParserTableWriter::replace_lookahead_accepts(std::string& str_) {
  std::vector<Bitset::ChunkType> accepts_flattened;

  size_t accept_count = 0;
  _writer_args->_tables._lookahead_state_machine.get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      State const& state = *_writer_args->_tables._lookahead_state_machine.get_state(state_nr_);
      size_t const cur = state.get_accepts().popcnt() == 0 ? 0 : state.get_accepts().countl(false);
      accept_count = std::max(accept_count, cur);
    }
   );

  _writer_args->_tables._lookahead_state_machine.get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      State const& state = *_writer_args->_tables._lookahead_state_machine.get_state(state_nr_);
      Bitset accepts = state.get_accepts();
      if (accepts.size() < accept_count) {
        accepts.resize(accept_count, false);
      }
      for (Bitset::ChunkType const& chunk : accepts.get_chunks()) {
        accepts_flattened.push_back(chunk);
      }
    }
   );
 
  std::stringstream accepts_replacement;
  TableWriterCommon::write_single_entries<Bitset::ChunkType>(accepts_replacement, accepts_flattened);
  replace_skeleton_label(str_, "LOOKAHEAD_ACCEPTS", accepts_replacement.str());
  replace_skeleton_label(str_, "LOOKAHEAD_ACCEPTS_CHUNK_COUNT", TableWriterCommon::as_hex(Bitset::get_required_chunk_count(accept_count), true));
}

void ParserTableWriter::replace_parser_accepts_labels(std::string& str_){
  std::vector<std::string> accepts_labels;
  accepts_labels.reserve(_writer_args->_tables._nonterminal_data.size());
  for (ParserTables::NonterminalData const& terminal_data : _writer_args->_tables._nonterminal_data) {
   accepts_labels.push_back(terminal_data._label);
  }

  std::stringstream terminal_labels_replacement;
  TableWriterCommon::write_single_entries(terminal_labels_replacement, accepts_labels);
  replace_skeleton_label(str_, "PARSER_ACCEPTS_LABELS", terminal_labels_replacement.str());
}

void ParserTableWriter::replace_parser_accepts_ids(std::string& str_) {
  std::vector<GenericId::IdType> ids;
  ids.reserve(_writer_args->_tables._nonterminal_data.size());
  for (ParserTables::NonterminalData const& terminal_data : _writer_args->_tables._nonterminal_data) {
   ids.push_back(terminal_data._id);
  }

  std::stringstream terminal_ids_replacement;
  TableWriterCommon::write_single_entries<GenericId::IdType>(terminal_ids_replacement, ids);
  replace_skeleton_label(str_, "PARSER_ACCEPTS_IDS", terminal_ids_replacement.str());
}

void ParserTableWriter::replace_id_names(std::string& str_) {
  std::stringstream id_names_replacement;
  TableWriterCommon::write_single_entries(id_names_replacement, _writer_args->_tables._id_names);
  replace_skeleton_label(str_, "ID_NAMES", id_names_replacement.str());
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
 for (size_t i = 0; i < _writer_args->_tables._nonterminal_data.size(); ++i) {
  unpack.push_back(_writer_args->_tables._nonterminal_data[i]._unpack);
 }

 std::vector<Bitset::ChunkType> unpack_flattened;
 for (Bitset::ChunkType const& chunk : unpack.get_chunks()) {
  unpack_flattened.push_back(chunk);
 }

 std::stringstream unpack_replacement;
 TableWriterCommon::write_single_entries<Bitset::ChunkType>(unpack_replacement, unpack_flattened);
 replace_skeleton_label(str_, "PARSER_UNPACK", unpack_replacement.str());
}

void ParserTableWriter::replace_parser_hide(std::string& str_) {
 Bitset hide;
 for (size_t i = 0; i < _writer_args->_tables._nonterminal_data.size(); ++i) {
  hide.push_back(_writer_args->_tables._nonterminal_data[i]._hide);
 }

 std::vector<Bitset::ChunkType> hide_flattened;
 for (Bitset::ChunkType const& chunk : hide.get_chunks()) {
  hide_flattened.push_back(chunk);
 }

 std::stringstream hide_replacement;
 TableWriterCommon::write_single_entries<Bitset::ChunkType>(hide_replacement, hide_flattened);
 replace_skeleton_label(str_, "PARSER_HIDE", hide_replacement.str());
}

void ParserTableWriter::replace_parser_merge(std::string& str_) {
 Bitset merge;
 for (size_t i = 0; i < _writer_args->_tables._nonterminal_data.size(); ++i) {
  merge.push_back(_writer_args->_tables._nonterminal_data[i]._merge);
 }

 std::vector<Bitset::ChunkType> merge_flattened;
 for (Bitset::ChunkType const& chunk : merge.get_chunks()) {
  merge_flattened.push_back(chunk);
 }

 std::stringstream merge_replacement;
 TableWriterCommon::write_single_entries<Bitset::ChunkType>(merge_replacement, merge_flattened);
 replace_skeleton_label(str_, "PARSER_MERGE", merge_replacement.str());
}

void ParserTableWriter::replace_id_constants(std::string& str_) {
 std::string id_constants_replacement;
 for (size_t i = 0; i < _writer_args->_tables._id_names.size(); ++i) {
  id_constants_replacement += _writer_args->_tables._id_names[i] + " = "  + TableWriterCommon::as_hex(i + _writer_args->_tables.get_min_id(), true) + ",\n";
 }

 replace_skeleton_label(str_, "ID_CONSTANTS", id_constants_replacement);
}

void ParserTableWriter::replace_timestamp(std::string& str_) {
  replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}

}