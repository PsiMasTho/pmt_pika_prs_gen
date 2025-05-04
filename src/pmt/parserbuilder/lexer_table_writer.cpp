#include "pmt/parserbuilder/lexer_table_writer.hpp"

#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/parserbuilder/table_writer_common.hpp"

#include <chrono>
#include <iomanip>
#include <vector>
#include <regex>
#include <sstream>
#include <limits>

namespace pmt::parserbuilder {
 using namespace pmt::util::smrt;
 using namespace pmt::util::smct;
 using namespace pmt::base;

 namespace {
  auto make_replace_regex(std::string const& label_) -> std::regex {
   std::string const pattern = R"(\/\*\s*\$replace\s*)" + label_ + R"(\s*\*\/)";
   return std::regex(pattern, std::regex::optimize);
  }

  auto get_smallest_unsigned_type(std::span<size_t const> data_) -> std::string {
   size_t const max = *std::max_element(data_.begin(), data_.end());
   if (max <= std::numeric_limits<uint8_t>::max()) {
     return "uint8_t";
   } else if (max <= std::numeric_limits<uint16_t>::max()) {
     return "uint16_t";
   } else if (max <= std::numeric_limits<uint32_t>::max()) {
     return "uint32_t";
   } else {
     return "uint64_t";
   }
  }
 }

 void LexerTableWriter::write(Arguments& args_) {
  _args = &args_;

  _header = std::string(std::istreambuf_iterator<char>(_args->_is_header_skel), std::istreambuf_iterator<char>());
  _source = std::string(std::istreambuf_iterator<char>(_args->_is_source_skel), std::istreambuf_iterator<char>());
  _id_constants = std::string(std::istreambuf_iterator<char>(_args->_is_id_constants_skel), std::istreambuf_iterator<char>());

  replace_in_header();
  replace_in_source();
  replace_in_id_constants();

  _args->_os_header << _header;
  _args->_os_source << _source;
  _args->_os_id_constants << _id_constants;
 }

 // ----------------------------
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
  replace_terminal_count(_source);
  replace_start_terminal_index(_source);
  replace_eoi_terminal_index(_source);
 }
 
 void LexerTableWriter::replace_in_id_constants(){
  replace_timestamp(_id_constants);
  replace_id_constants(_id_constants);
 }
 
 void LexerTableWriter::replace_namespace_open(std::string& str_) {
  if (_args->_namespace_name.empty()) {
    return;
  }

  std::string const namespace_open_replacement = "namespace " + _args->_namespace_name + " {";
  static std::regex const namespace_open_regex = make_replace_regex("NAMESPACE_OPEN");
  str_ = std::regex_replace(str_, namespace_open_regex, namespace_open_replacement);
 }
 
 void LexerTableWriter::replace_class_name(std::string& str_){
  static std::regex const class_name_regex = make_replace_regex("CLASS_NAME");
  str_ = std::regex_replace(str_, class_name_regex, _args->_class_name);
 }

 void LexerTableWriter::replace_namespace_close(std::string& str_){
  if (_args->_namespace_name.empty()) {
    return;
  }

  std::string const namespace_close_replacement = "} // namespace " + _args->_namespace_name + "\n";
  static std::regex const namespace_close_regex = make_replace_regex("NAMESPACE_CLOSE");
  str_ = std::regex_replace(str_, namespace_close_regex, namespace_close_replacement);
 }
 
 void LexerTableWriter::replace_header_include_path(std::string& str_){
  static std::regex const header_include_path_regex = make_replace_regex("HEADER_INCLUDE_PATH");
  str_ = std::regex_replace(str_, header_include_path_regex, _args->_header_include_path);
 }
 
 void LexerTableWriter::replace_transitions(std::string& str_){
  std::vector<SymbolType> lower_bounds;
  std::vector<SymbolType> upper_bounds;
  std::vector<StateNrType> values;
  std::vector<size_t> offsets{0};
 
   _args->_tables.get_state_nrs().for_each_key(
     [&](pmt::util::smrt::StateNrType state_nr_) {
       State const& state = *_args->_tables.get_state(state_nr_);
       IntervalSet<SymbolType> const& symbols = state.get_symbols();
       offsets.push_back(offsets.back() + symbols.size());
       symbols.for_each_interval(
         [&](Interval<SymbolType> interval_) {
           // Store lowers and uppers + 1, this will make the EOI symbol
           // roll over to zero and keep all values small in common cases.
           // This is undone in the generated lexer class by subtracting where needed.
           // This is all done so that we can use smaller types for the bounds, making the
           // tables smaller and the search faster.
           lower_bounds.push_back(interval_.get_lower() + 1);
           upper_bounds.push_back(interval_.get_upper() + 1);
           StateNrType const value = state.get_symbol_transition(Symbol(interval_.get_lower()));
           values.push_back(value);
         }
       );
     }
   );

   {
    std::stringstream lower_bounds_replacement;
    TableWriterCommon::write_single_entries<SymbolType>(lower_bounds_replacement, lower_bounds);
    static std::regex const lower_bounds_regex = make_replace_regex("LOWER_BOUNDS");
    str_ = std::regex_replace(str_, lower_bounds_regex, lower_bounds_replacement.str());
   }
   std::string const lower_bounds_type_replacement = get_smallest_unsigned_type(lower_bounds);
   static std::regex const lower_bounds_type_regex = make_replace_regex("LOWER_BOUNDS_TYPE");
   str_ = std::regex_replace(str_, lower_bounds_type_regex, lower_bounds_type_replacement);
   std::stringstream upper_bounds_replacement;
   {
    TableWriterCommon::write_single_entries<SymbolType>(upper_bounds_replacement, upper_bounds);
    static std::regex const upper_bounds_regex = make_replace_regex("UPPER_BOUNDS");
    str_ = std::regex_replace(str_, upper_bounds_regex, upper_bounds_replacement.str());
   }
   std::string const upper_bounds_type_replacement = get_smallest_unsigned_type(upper_bounds);
   static std::regex const upper_bounds_type_regex = make_replace_regex("UPPER_BOUNDS_TYPE");
   str_ = std::regex_replace(str_, upper_bounds_type_regex, upper_bounds_type_replacement);
   std::stringstream values_replacement;
   {
    TableWriterCommon::write_single_entries<StateNrType>(values_replacement, values);
    static std::regex const values_regex = make_replace_regex("VALUES");
    str_ = std::regex_replace(str_, values_regex, values_replacement.str());
   }
   std::string const values_type_replacement = get_smallest_unsigned_type(values);
   static std::regex const values_type_regex = make_replace_regex("VALUES_TYPE");
   str_ = std::regex_replace(str_, values_type_regex, values_type_replacement);
   std::stringstream offsets_replacement;
   {
    TableWriterCommon::write_single_entries<size_t>(offsets_replacement, offsets);
    static std::regex const offsets_regex = make_replace_regex("OFFSETS");
    str_ = std::regex_replace(str_, offsets_regex, offsets_replacement.str());
   }
   std::string const offsets_type_replacement = get_smallest_unsigned_type(offsets);
   static std::regex const offsets_type_regex = make_replace_regex("OFFSETS_TYPE");
   str_ = std::regex_replace(str_, offsets_type_regex, offsets_type_replacement);
 }
 
 void LexerTableWriter::replace_terminals(std::string& str_){
  std::vector<Bitset::ChunkType> terminals_flattened;

  size_t const terminal_count = _args->_tables.get_terminal_count();

  _args->_tables.get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      State const& state = *_args->_tables.get_state(state_nr_);
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
  static std::regex const terminals_regex = make_replace_regex("ACCEPTS");
  str_ = std::regex_replace(str_, terminals_regex, terminals_replacement.str());
 }
 
 void LexerTableWriter::replace_terminal_labels(std::string& str_){
  std::vector<std::string> terminal_labels;
  terminal_labels.reserve(_args->_tables._terminal_data.size());
  for (LexerTables::TerminalData const& terminal_data : _args->_tables._terminal_data) {
   terminal_labels.push_back(terminal_data._label);
  }

  std::stringstream terminal_labels_replacement;
  TableWriterCommon::write_single_entries(terminal_labels_replacement, terminal_labels);
  static std::regex const terminal_labels_regex = make_replace_regex("TERMINAL_LABELS");
  str_ = std::regex_replace(str_, terminal_labels_regex, terminal_labels_replacement.str());
 }
 
 void LexerTableWriter::replace_terminal_ids(std::string& str_){
  std::vector<GenericId::IdType> terminal_ids;
  terminal_ids.reserve(_args->_tables._terminal_data.size());
  for (LexerTables::TerminalData const& terminal_data : _args->_tables._terminal_data) {
   terminal_ids.push_back(terminal_data._id);
  }

  std::stringstream terminal_ids_replacement;
  TableWriterCommon::write_single_entries<GenericId::IdType>(terminal_ids_replacement, terminal_ids);
  static std::regex const terminal_ids_regex = make_replace_regex("TERMINAL_IDS");
  str_ = std::regex_replace(str_, terminal_ids_regex, terminal_ids_replacement.str());
 }
 
 void LexerTableWriter::replace_id_names(std::string& str_){
  std::stringstream id_names_replacement;
  TableWriterCommon::write_single_entries(id_names_replacement, _args->_tables._id_names);
  static std::regex const id_names_regex = make_replace_regex("ID_NAMES");
  str_ = std::regex_replace(str_, id_names_regex, id_names_replacement.str());
 }
 
 void LexerTableWriter::replace_terminal_count(std::string& str_){
  std::string terminal_count_replacement = TableWriterCommon::as_hex(_args->_tables.get_terminal_count(), true);
  static std::regex const terminal_count_regex = make_replace_regex("TERMINAL_COUNT");
  str_ = std::regex_replace(str_, terminal_count_regex, terminal_count_replacement);
 }
 
 void LexerTableWriter::replace_start_terminal_index(std::string& str_){
  std::string start_terminal_index_replacement = TableWriterCommon::as_hex(_args->_tables.get_start_terminal_index(), true);
  static std::regex const start_terminal_index_regex = make_replace_regex("START_TERMINAL_INDEX");
  str_ = std::regex_replace(str_, start_terminal_index_regex, start_terminal_index_replacement);
 }
 
 void LexerTableWriter::replace_eoi_terminal_index(std::string& str_){
  std::string eoi_terminal_index_replacement = TableWriterCommon::as_hex(_args->_tables.get_eoi_terminal_index(), true);
  static std::regex const eoi_terminal_index_regex = make_replace_regex("EOI_TERMINAL_INDEX");
  str_ = std::regex_replace(str_, eoi_terminal_index_regex, eoi_terminal_index_replacement);
 }

void LexerTableWriter::replace_id_constants(std::string& str_) {
 std::string id_constants_replacement;
 for (size_t i = 0; i < _args->_tables._id_names.size(); ++i) {
  id_constants_replacement += _args->_tables._id_names[i] + " = "  + TableWriterCommon::as_hex(i, true) + ",\n";
 }

 static std::regex const id_constants_regex = make_replace_regex("ID_CONSTANTS");
 str_ = std::regex_replace(str_, id_constants_regex, id_constants_replacement);
}

void LexerTableWriter::replace_timestamp(std::string& str_) {
  auto const now = std::chrono::system_clock::now();
  auto const time = std::chrono::system_clock::to_time_t(now);
  auto const local_time = *std::localtime(&time);

  std::stringstream timestamp_replacement;
  timestamp_replacement << "/* Generated on: " << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S") << " */";
  static std::regex const timestamp_regex = make_replace_regex("TIMESTAMP");
  str_ = std::regex_replace(str_, timestamp_regex, timestamp_replacement.str());
}

}