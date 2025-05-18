#include "pmt/parserbuilder/lexer_table_writer.hpp"

#include "pmt/util/timestamp.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/parserbuilder/table_writer_common.hpp"

#include <set>
#include <vector>
#include <sstream>

namespace pmt::parserbuilder {
 using namespace pmt::util::smrt;
 using namespace pmt::util::smct;
 using namespace pmt::base;

 namespace {
  auto encode_symbol(pmt::util::smrt::SymbolType symbol_) -> pmt::util::smrt::SymbolType {
   return (symbol_ == pmt::util::smrt::SymbolValueMax) ? 0 : symbol_ + 1;
  }
 }

 void LexerTableWriter::write(WriterArgs& writer_writer_args_) {
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
  replace_terminals(_source);
  replace_terminal_labels(_source);
  replace_terminal_ids(_source);
  replace_id_names(_source);
  replace_min_id(_source);
  replace_id_count(_source);
  replace_hides(_source);
  replace_linecount_transitions(_source);
  replace_linecount_accepts(_source);
  replace_terminal_count(_source);
  replace_start_terminal_index(_source);
  replace_eoi_terminal_index(_source);
 }
 
 void LexerTableWriter::replace_in_id_constants(){
  replace_timestamp(_id_constants);
  replace_id_constants(_id_constants);
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
  std::vector<SymbolType> lower_bounds;
  std::vector<SymbolType> upper_bounds;
  std::vector<StateNrType> values;
  std::vector<size_t> offsets{0};
 
   _writer_args->_tables._lexer_state_machine.get_state_nrs().for_each_key(
     [&](pmt::util::smrt::StateNrType state_nr_) {
       State const& state = *_writer_args->_tables._lexer_state_machine.get_state(state_nr_);
       IntervalMap<SymbolType, StateNrType> const& symbol_transitions = state.get_symbol_transitions();
       symbol_transitions.for_each_interval([&](StateNrType value_, Interval<SymbolType> interval_){
           // Store lowers and uppers + 1, this will make the EOI symbol
           // roll over to zero and keep all values small in common cases.
           // This is undone in the generated lexer class by subtracting where needed.
           // This is all done so that we can use smaller types for the bounds, making the
           // tables smaller and the search faster.
           lower_bounds.push_back(encode_symbol(interval_.get_lower()));
           upper_bounds.push_back(encode_symbol(interval_.get_upper()));
           values.push_back(value_);
       });
       offsets.push_back(offsets.back() + symbol_transitions.size());
     }
   );

   {
    std::stringstream lower_bounds_replacement;
    TableWriterCommon::write_single_entries<SymbolType>(lower_bounds_replacement, lower_bounds);
    replace_skeleton_label(str_, "LOWER_BOUNDS", lower_bounds_replacement.str());
   }
   std::string const lower_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(lower_bounds);
   replace_skeleton_label(str_, "LOWER_BOUNDS_TYPE", lower_bounds_type_replacement);
   
   std::stringstream upper_bounds_replacement;
   {
    TableWriterCommon::write_single_entries<SymbolType>(upper_bounds_replacement, upper_bounds);
    replace_skeleton_label(str_, "UPPER_BOUNDS", upper_bounds_replacement.str());
   }
   std::string const upper_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(upper_bounds);
   replace_skeleton_label(str_, "UPPER_BOUNDS_TYPE", upper_bounds_type_replacement);

   std::stringstream values_replacement;
   {
    TableWriterCommon::write_single_entries<StateNrType>(values_replacement, values);
    replace_skeleton_label(str_, "VALUES", values_replacement.str());
   }
   std::string const values_type_replacement = TableWriterCommon::get_smallest_unsigned_type<StateNrType>(values);
   replace_skeleton_label(str_, "VALUES_TYPE", values_type_replacement);

   std::stringstream offsets_replacement;
   {
    TableWriterCommon::write_single_entries<size_t>(offsets_replacement, offsets);
    replace_skeleton_label(str_, "OFFSETS", offsets_replacement.str());
   }
   std::string const offsets_type_replacement = TableWriterCommon::get_smallest_unsigned_type<size_t>(offsets);
   replace_skeleton_label(str_, "OFFSETS_TYPE", offsets_type_replacement);
 }
 
 void LexerTableWriter::replace_terminals(std::string& str_){
  std::vector<Bitset::ChunkType> terminals_flattened;

  size_t const terminal_count = _writer_args->_tables.get_accept_count();

  _writer_args->_tables._lexer_state_machine.get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      State const& state = *_writer_args->_tables._lexer_state_machine.get_state(state_nr_);
      Bitset terminals = state.get_accepts();
      if (terminals.size() < terminal_count) {
        terminals.resize(terminal_count, false);
      }
      for (Bitset::ChunkType const& chunk : terminals.get_chunks()) {
        terminals_flattened.push_back(chunk);
      }
    }
   );
 
  std::stringstream terminals_replacement;
  TableWriterCommon::write_single_entries<Bitset::ChunkType>(terminals_replacement, terminals_flattened);
  replace_skeleton_label(str_, "TERMINALS", terminals_replacement.str());
 }
 
 void LexerTableWriter::replace_terminal_labels(std::string& str_){
  std::vector<std::string> terminal_labels;
  terminal_labels.reserve(_writer_args->_tables._terminal_data.size());
  for (LexerTables::TerminalData const& terminal_data : _writer_args->_tables._terminal_data) {
   terminal_labels.push_back(terminal_data._label);
  }

  std::stringstream terminal_labels_replacement;
  TableWriterCommon::write_single_entries(terminal_labels_replacement, terminal_labels);
  replace_skeleton_label(str_, "TERMINAL_LABELS", terminal_labels_replacement.str());
 }
 
 void LexerTableWriter::replace_terminal_ids(std::string& str_){
  std::vector<GenericId::IdType> terminal_ids;
  terminal_ids.reserve(_writer_args->_tables._terminal_data.size());
  for (LexerTables::TerminalData const& terminal_data : _writer_args->_tables._terminal_data) {
   terminal_ids.push_back(terminal_data._id);
  }

  std::stringstream terminal_ids_replacement;
  TableWriterCommon::write_single_entries<GenericId::IdType>(terminal_ids_replacement, terminal_ids);
  replace_skeleton_label(str_, "TERMINAL_IDS", terminal_ids_replacement.str());
 }
 
 void LexerTableWriter::replace_id_names(std::string& str_){
  std::stringstream id_names_replacement;
  TableWriterCommon::write_single_entries(id_names_replacement, _writer_args->_tables._id_names);
  replace_skeleton_label(str_, "ID_NAMES", id_names_replacement.str());
 }

 void LexerTableWriter::replace_min_id(std::string& str_) {
  std::string const min_id_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_min_id(), true);
  replace_skeleton_label(str_, "MIN_ID", min_id_replacement);
}

void LexerTableWriter::replace_id_count(std::string& str_) {
  std::string const id_count_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_id_count(), true);
  replace_skeleton_label(str_, "ID_COUNT", id_count_replacement);
}

void LexerTableWriter::replace_hides(std::string& str_) {
 Bitset hide;
 for (size_t i = 0; i < _writer_args->_tables._terminal_data.size(); ++i) {
  hide.push_back(_writer_args->_tables._terminal_data[i]._hide);
 }

 std::vector<Bitset::ChunkType> hide_flattened;
 for (Bitset::ChunkType const& chunk : hide.get_chunks()) {
  hide_flattened.push_back(chunk);
 }

 std::stringstream hide_replacement;
 TableWriterCommon::write_single_entries<Bitset::ChunkType>(hide_replacement, hide_flattened);
 replace_skeleton_label(str_, "HIDES", hide_replacement.str());
}


 void LexerTableWriter::replace_linecount_transitions(std::string& str_) {
  std::vector<SymbolType> linecount_lower_bounds;
  std::vector<SymbolType> linecount_upper_bounds;
  std::vector<StateNrType> linecount_values;
  std::vector<size_t> linecount_offsets{0};
 
   _writer_args->_tables._linecount_state_machine.get_state_nrs().for_each_key(
     [&](pmt::util::smrt::StateNrType state_nr_) {
       State const& state = *_writer_args->_tables._linecount_state_machine.get_state(state_nr_);
       IntervalMap<SymbolType, StateNrType> const& symbol_transitions = state.get_symbol_transitions();
       linecount_offsets.push_back(linecount_offsets.back() + symbol_transitions.size());
       symbol_transitions.for_each_interval(
         [&](StateNrType value_, Interval<SymbolType> interval_) {
           linecount_lower_bounds.push_back(encode_symbol(interval_.get_lower()));
           linecount_upper_bounds.push_back(encode_symbol(interval_.get_upper()));
           linecount_values.push_back(value_);
         }
       );
     }
   );

   {
    std::stringstream lower_bounds_replacement;
    TableWriterCommon::write_single_entries<SymbolType>(lower_bounds_replacement, linecount_lower_bounds);
    replace_skeleton_label(str_, "LINECOUNT_LOWER_BOUNDS", lower_bounds_replacement.str());
   }
   std::string const lower_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(linecount_lower_bounds);
   replace_skeleton_label(str_, "LINECOUNT_LOWER_BOUNDS_TYPE", lower_bounds_type_replacement);
   
   std::stringstream upper_bounds_replacement;
   {
    TableWriterCommon::write_single_entries<SymbolType>(upper_bounds_replacement, linecount_upper_bounds);
    replace_skeleton_label(str_, "LINECOUNT_UPPER_BOUNDS", upper_bounds_replacement.str());
   }
   std::string const upper_bounds_type_replacement = TableWriterCommon::get_smallest_unsigned_type<SymbolType>(linecount_upper_bounds);
   replace_skeleton_label(str_, "LINECOUNT_UPPER_BOUNDS_TYPE", upper_bounds_type_replacement);

   std::stringstream values_replacement;
   {
    TableWriterCommon::write_single_entries<StateNrType>(values_replacement, linecount_values);
    replace_skeleton_label(str_, "LINECOUNT_VALUES", values_replacement.str());
   }
   std::string const values_type_replacement = TableWriterCommon::get_smallest_unsigned_type<StateNrType>(linecount_values);
   replace_skeleton_label(str_, "LINECOUNT_VALUES_TYPE", values_type_replacement);

   std::stringstream offsets_replacement;
   {
    TableWriterCommon::write_single_entries<size_t>(offsets_replacement, linecount_offsets);
    replace_skeleton_label(str_, "LINECOUNT_OFFSETS", offsets_replacement.str());
   }
   std::string const offsets_type_replacement = TableWriterCommon::get_smallest_unsigned_type<size_t>(linecount_offsets);
   replace_skeleton_label(str_, "LINECOUNT_OFFSETS_TYPE", offsets_type_replacement);
 }

 void LexerTableWriter::replace_linecount_accepts(std::string& str_) {
  std::set<StateNrType> linecount_accepts;
  _writer_args->_tables._linecount_state_machine.get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      if (_writer_args->_tables._linecount_state_machine.get_state(state_nr_)->get_accepts().any()) {
        linecount_accepts.insert(state_nr_);
      }
    }
   );

   {
   std::stringstream linecount_accepts_replacement;
   TableWriterCommon::write_single_entries(linecount_accepts_replacement, linecount_accepts.begin(), linecount_accepts.end());
   replace_skeleton_label(str_, "LINECOUNT_ACCEPTS", linecount_accepts_replacement.str());
  }
  std::string const linecount_accepts_type_replacement = TableWriterCommon::get_smallest_unsigned_type(linecount_accepts.begin(), linecount_accepts.end());
  replace_skeleton_label(str_, "LINECOUNT_ACCEPTS_TYPE", linecount_accepts_type_replacement);   
  replace_skeleton_label(str_, "LINECOUNT_ACCEPTS_COUNT", TableWriterCommon::as_hex(linecount_accepts.size(), true));
 }
 
 void LexerTableWriter::replace_terminal_count(std::string& str_){
  size_t const accept_count = _writer_args->_tables.get_accept_count();
  replace_skeleton_label(str_, "TERMINAL_COUNT", TableWriterCommon::as_hex(accept_count, true));
  replace_skeleton_label(str_, "TERMINAL_CHUNK_COUNT", TableWriterCommon::as_hex(Bitset::get_required_chunk_count(accept_count), true));
 }
 
 void LexerTableWriter::replace_start_terminal_index(std::string& str_){
  std::string start_terminal_index_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_start_accept_index(), true);
  replace_skeleton_label(str_, "START_TERMINAL_INDEX", start_terminal_index_replacement);
 }
 
 void LexerTableWriter::replace_eoi_terminal_index(std::string& str_){
  std::string eoi_terminal_index_replacement = TableWriterCommon::as_hex(_writer_args->_tables.get_eoi_accept_index(), true);
  replace_skeleton_label(str_, "EOI_TERMINAL_INDEX", eoi_terminal_index_replacement);
 }

void LexerTableWriter::replace_id_constants(std::string& str_) {
 std::string id_constants_replacement;
 for (GenericId::IdType i = 0; i < _writer_args->_tables._id_names.size(); ++i) {
  id_constants_replacement += _writer_args->_tables._id_names[i] + " = "  + TableWriterCommon::as_hex(i + _writer_args->_tables.get_min_id(), true) + ",\n";
 }

 replace_skeleton_label(str_, "ID_CONSTANTS", id_constants_replacement);
}

void LexerTableWriter::replace_timestamp(std::string& str_) {
  replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}

}