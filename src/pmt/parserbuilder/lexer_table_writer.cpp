#include "pmt/parserbuilder/lexer_table_writer.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parserbuilder/lexer_tables.hpp"
#include "pmt/parserbuilder/table_writer_common.hpp"

#include <vector>

namespace pmt::parserbuilder {
 using namespace pmt::util::smrt;
 using namespace pmt::util::smct;
 using namespace pmt::base;

 LexerTableWriter::LexerTableWriter(std::ostream& os_header_, std::ostream& os_source_, std::ostream& os_id_constants_, LexerTables const& tables_) 
 : _os_header(os_header_)
 , _os_source(os_source_)
 , _os_id_constants(os_id_constants_)
 , _tables(tables_)
 , _args(nullptr) {
 }

 void LexerTableWriter::write(Arguments const& args_) {
  _args = &args_;

  write_header();
  write_source();
  write_id_constants();
 }

 void LexerTableWriter::write_header() {
  write_header_include_guard_top();
  write_header_includes();
  write_namespace_top(_os_header);

  write_header_class_top();
  write_header_class_body();
  write_header_class_bottom();
  
  write_namespace_bottom(_os_header);
  write_header_include_guard_bottom();
 }

 void LexerTableWriter::write_source() {
  write_source_includes();
  write_namespace_top(_os_source);
  write_anonymous_namespace_top(_os_source);
  write_source_transitions();
  write_source_accepts();
  write_source_id_names();
  write_anonymous_namespace_bottom(_os_source);
  write_source_function_definitions();
  write_namespace_bottom(_os_source);
 }

 void LexerTableWriter::write_id_constants() {
  // Note: always add a comma at the end because we don't know what comes after
  for (size_t i = 0; i < _tables._id_names.size(); ++i) {
    _os_id_constants << _tables._id_names[i] << " = "  << TableWriterCommon::as_hex(i, true, true) << ",\n";
  }
 }

 void LexerTableWriter::write_header_includes() {
  write_include_statement(_os_header, "pmt/base/bitset.hpp", _args->_pmt_lib_include_quotes);
  write_include_statement(_os_header, "pmt/util/smrt/lexer_tables_base.hpp", _args->_pmt_lib_include_quotes);
  _os_header << "\n";
  write_include_statement(_os_header, "string", IncludeQuotes::Angle);
  _os_header << "\n";
 }

 void LexerTableWriter::write_anonymous_namespace_top(std::ostream& os_){
  os_ << get_indent() << "namespace {\n";
 }

 void LexerTableWriter::write_anonymous_namespace_bottom(std::ostream& os_){
  os_ << get_indent() << "}\n\n";
 }

 void LexerTableWriter::write_namespace_top(std::ostream& os_) {
  if (!_args->_namespace_name.empty()) {
    os_ << get_indent() << "namespace " << _args->_namespace_name << " {\n\n";
  }
 }

 void LexerTableWriter::write_namespace_bottom(std::ostream& os_) {
  if (!_args->_namespace_name.empty()) {
    os_ << get_indent() << "}\n\n";
  }
 }


 void LexerTableWriter::write_header_class_top() {
  _os_header << "class " << _args->_class_name << " : public pmt::util::smrt::LexerTablesBase {\n";
  increment_indent();
 }

 void LexerTableWriter::write_header_class_body() {
  _os_header << get_indent() << "public:\n";
  increment_indent();
  _os_header << get_indent() << "// -$ Functions $-\n";
  _os_header << get_indent() << "// --$ Inherited: pmt::util::smrt::LexerTablesBase $--\n";
  _os_header << get_indent() << "auto get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType override;\n";
  _os_header << get_indent() << "auto get_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst override;\n";
  _os_header << get_indent() << "auto get_accept_count() const -> size_t override;\n";
  _os_header << get_indent() << "auto get_start_accept_index() const -> size_t override;\n";
  _os_header << get_indent() << "auto get_eoi_accept_index() const -> size_t override;\n";
  _os_header << get_indent() << "auto get_accept_string(size_t index_) const -> std::string override;\n";
  _os_header << get_indent() << "auto get_accept_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType override;\n";
  _os_header << get_indent() << "auto id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string override;\n";
  decrement_indent();
 }

 void LexerTableWriter::write_header_class_bottom() {
  _os_header << "};\n";
  decrement_indent();
 }
 
 void LexerTableWriter::write_header_include_guard_top() {
  if (_args->_header_include_guard_define.empty()) {
    _os_header << "#pragma once\n\n";
  } else {
    _os_header << "#ifndef " << _args->_header_include_guard_define << "\n";
    _os_header << "#define " << _args->_header_include_guard_define << "\n\n";
  }
 }

 void LexerTableWriter::write_header_include_guard_bottom() {
  if (!_args->_header_include_guard_define.empty()) {
    _os_header << "#endif // " << _args->_header_include_guard_define << "\n";
  }
 }

void LexerTableWriter::write_source_includes() {
  write_include_statement(_os_source, _args->_header_include_path, _args->_generated_header_include_quotes);
  _os_source << "\n";
}

void LexerTableWriter::write_source_transitions() {
 std::vector<size_t> transitions_lower_bounds;
 std::vector<size_t> transitions_upper_bounds;
 std::vector<size_t> transitions_values;
 std::vector<size_t> transition_offsets{0};

  _tables.get_state_nrs().for_each_key(
    [&](pmt::util::smrt::StateNrType state_nr_) {
      State const& state = *_tables.get_state(state_nr_);
      IntervalSet<SymbolType> const& symbols = state.get_symbols();
      transition_offsets.push_back(transition_offsets.back() + symbols.size());
      symbols.for_each_interval(
        [&](Interval<SymbolType> interval_) {
          transitions_lower_bounds.push_back(interval_.get_lower());
          transitions_upper_bounds.push_back(interval_.get_upper());
          size_t const value = state.get_symbol_transition(Symbol(interval_.get_lower()));
          transitions_values.push_back(value);
        }
      );
    }
  );

  std::string const transitions_lower_bound_label = "pmt::util::smrt::SymbolType const TRANSITIONS_LOWER_BOUNDS[]";
  TableWriterCommon::write_single_entries<size_t>(_os_source, transitions_lower_bounds, transitions_lower_bound_label);
  _os_source << "\n";

  std::string const transitions_upper_bound_label = "pmt::util::smrt::SymbolType const TRANSITIONS_UPPER_BOUNDS[]";
  TableWriterCommon::write_single_entries<size_t>(_os_source, transitions_upper_bounds, transitions_upper_bound_label);
  _os_source << "\n";

  std::string const transitions_values_label = "pmt::util::smrt::StateNrType const TRANSITIONS_VALUES[]";
  TableWriterCommon::write_single_entries<size_t>(_os_source, transitions_values, transitions_values_label);
  _os_source << "\n";

  std::string const transition_offsets_label = "size_t const TRANSITIONS_OFFSETS[]";
  TableWriterCommon::write_single_entries<size_t>(_os_source, transition_offsets, transition_offsets_label);
  _os_source << "\n";
}

void LexerTableWriter::write_source_accepts() {
 size_t const accept_count = _tables.get_accept_count();
 std::vector<Bitset::ChunkType> accepts_flattened;

 _tables.get_state_nrs().for_each_key(
   [&](pmt::util::smrt::StateNrType state_nr_) {
     State const& state = *_tables.get_state(state_nr_);
     Bitset accepts = state.get_accepts();
     if (accepts.size() < accept_count) {
       accepts.resize(accept_count, false);
     }
     for (Bitset::ChunkType const& chunk : accepts.get_chunks()) {
       accepts_flattened.push_back(chunk);
     }
   }
  );

  std::string const accept_count_label = "size_t const ACCEPT_COUNT";
  _os_source << get_indent() << accept_count_label << " = " << TableWriterCommon::as_hex(accept_count, true, true) << ";\n\n";

  std::string const start_accept_index_label = "size_t const START_ACCEPT_INDEX";
  _os_source << get_indent() << start_accept_index_label << " = " << TableWriterCommon::as_hex(_tables.get_start_accept_index(), true, true) << ";\n\n";

  std::string const eoi_accept_index_label = "size_t const EOI_ACCEPT_INDEX";
  _os_source << get_indent() << eoi_accept_index_label << " = " << TableWriterCommon::as_hex(_tables.get_eoi_accept_index(), true, true) << ";\n\n";

  TableWriterCommon::write_single_entries<Bitset::ChunkType>(_os_source, accepts_flattened, "pmt::base::Bitset::ChunkType const ACCEPTS[]");
  _os_source << '\n';

  std::vector<std::string> accept_strings;
  std::vector<GenericId::IdType> accept_ids;

  for (LexerTables::TerminalData const& terminal_data : _tables._terminal_data) {
   accept_strings.push_back(terminal_data._name);
   accept_ids.push_back(terminal_data._id);
  }

  std::string const accept_strings_label = "char const* const ACCEPT_NAMES[]";
  TableWriterCommon::write_single_entries(_os_source, accept_strings, accept_strings_label);
  _os_source << '\n';

  std::string const accept_ids_label = "pmt::util::smrt::GenericId::IdType const ACCEPT_IDS[]";
  TableWriterCommon::write_single_entries<GenericId::IdType>(_os_source, accept_ids, accept_ids_label);
  _os_source << '\n';
}

void LexerTableWriter::write_source_id_names() {
 std::string const id_names_label = "char const* const ID_NAMES[]";
 TableWriterCommon::write_single_entries(_os_source, _tables._id_names, id_names_label);
 _os_source << '\n';
}

void LexerTableWriter::write_source_function_definitions() {
 write_source_get_state_nr_next();
 write_source_get_state_accepts();
 write_source_get_accept_count();
 write_source_get_start_accept_index();
 write_source_get_eoi_accept_index();
 write_source_get_accept_string();
 write_source_get_accept_id();
 write_source_id_to_string();
}

void LexerTableWriter::write_source_get_state_nr_next() {
 _os_source << get_indent() << "auto " << _args->_class_name << "::get_state_nr_next(pmt::util::smrt::StateNrType state_nr_, pmt::util::smrt::SymbolType symbol_) const -> pmt::util::smrt::StateNrType {\n";
 increment_indent();
 _os_source << get_indent() << "return pmt::util::smrt::LexerTablesBase::get_state_nr_next_impl(state_nr_, symbol_, TRANSITIONS_LOWER_BOUNDS, TRANSITIONS_UPPER_BOUNDS, TRANSITIONS_VALUES, TRANSITIONS_OFFSETS);\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

void LexerTableWriter::write_source_get_state_accepts() {
 _os_source << get_indent() << "auto " << _args->_class_name << "::get_state_accepts(pmt::util::smrt::StateNrType state_nr_) const -> pmt::base::Bitset::ChunkSpanConst {\n";
 increment_indent();
 _os_source << get_indent() << "return pmt::base::Bitset::ChunkSpanConst(ACCEPTS + state_nr_ * ACCEPT_COUNT, ACCEPT_COUNT);\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

void LexerTableWriter::write_source_get_accept_count(){
 _os_source << get_indent() << "auto " << _args->_class_name << "::get_accept_count() const -> size_t {\n";
 increment_indent();
 _os_source << get_indent() << "return ACCEPT_COUNT;\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

void LexerTableWriter::write_source_get_start_accept_index(){
 _os_source << get_indent() << "auto " << _args->_class_name << "::get_start_accept_index() const -> size_t {\n";
 increment_indent();
 _os_source << get_indent() << "return START_ACCEPT_INDEX;\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

void LexerTableWriter::write_source_get_eoi_accept_index(){
 _os_source << get_indent() << "auto " << _args->_class_name << "::get_eoi_accept_index() const -> size_t {\n";
 increment_indent();
 _os_source << get_indent() << "return EOI_ACCEPT_INDEX;\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

void LexerTableWriter::write_source_get_accept_string() {
 _os_source << get_indent() << "auto " << _args->_class_name << "::get_accept_string(size_t index_) const -> std::string {\n";
 increment_indent();
 _os_source << get_indent() << "return ACCEPT_NAMES[index_];\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

void LexerTableWriter::write_source_get_accept_id() {
 _os_source << get_indent() << "auto " << _args->_class_name << "::get_accept_id(size_t index_) const -> pmt::util::smrt::GenericId::IdType {\n";
 increment_indent();
 _os_source << get_indent() << "return ACCEPT_IDS[index_];\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

void LexerTableWriter::write_source_id_to_string() {
 _os_source << get_indent() << "auto " << _args->_class_name << "::id_to_string(pmt::util::smrt::GenericId::IdType id_) const -> std::string {\n";
 increment_indent();
 _os_source << get_indent() << "return ID_NAMES[id_];\n";
 decrement_indent();
 _os_source << get_indent() << "}\n\n";
}

 auto LexerTableWriter::get_indent() -> std::string {
  return std::string(_indent, ' ');
 }

 void LexerTableWriter::increment_indent() {
  _indent += _args->_indent_increment;
 }

 void LexerTableWriter::decrement_indent() {
  size_t const decr = std::min(_args->_indent_increment, _indent);
  _indent -= decr;
 }

void LexerTableWriter::write_include_statement(std::ostream& os_, std::string const& include_path_, IncludeQuotes quotes_) {
  os_ << get_indent() << "#include " << get_open_quote(quotes_) << include_path_ << get_close_quote(quotes_) << "\n";
}

 auto LexerTableWriter::get_open_quote(IncludeQuotes quotes_) -> char {
  switch (quotes_) {
    case IncludeQuotes::Angle:
      return '<';
    case IncludeQuotes::DoubleQuote:
      return '"';
    default:
    pmt::unreachable();
  }
 }
 
 auto LexerTableWriter::get_close_quote(IncludeQuotes quotes_) -> char {
  switch (quotes_) {
    case IncludeQuotes::Angle:
      return '>';
    case IncludeQuotes::DoubleQuote:
      return '"';
    default:
    pmt::unreachable();
  }
 }

}