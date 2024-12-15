#include "pmt/parserbuilder/table_writer.hpp"

#include "pmt/parserbuilder/lexer_tables.hpp"

#include <sstream>
#include <utility>

namespace pmt::parserbuilder {
using namespace pmt::util::parse;

namespace {}  // namespace

TableWriter::TableWriter(std::ostream& os_header_, std::ostream& os_source_, std::string header_path_, std::string namespace_name_, std::string class_name_, LexerTables const& tables_)
 : _os_header(os_header_)
 , _os_source(os_source_)
 , _tables(tables_)
 , _header_path(std::move(header_path_))
 , _namespace_name(std::move(namespace_name_))
 , _class_name(std::move(class_name_)) {
}

void TableWriter::write() {
  write_header();
  write_source();
}

void TableWriter::write_header() {
  // clang-format off
  _os_header << "#pragma once\n"
  "\n"
  R"(#include "pmt/util/parsert/generic_id.hpp")"
  "\n"
  R"(#include "pmt/util/parsert/generic_lexer_tables.hpp")"
  "\n"
  "\n";
  _os_header << _namespace_name << " {\n"
  "\n"
  "class " << _class_name << " {\n"
  " public:\n"
  "  auto id_to_string(pmt::util::parsert::GenericId::IdType id_) -> std::string_view;\n"
  "  auto as_generic_lexer_tables() const -> pmt::util::parsert::GenericLexerTables;\n"
  "\n";
  write_header_id_enum();
  _os_header << "};\n";
  _os_header << "}";
  // clang-format off
}

void TableWriter::write_source() {
  // clang-format off
  _os_source << "#include \"" << _header_path << "\"\n"
  "\n"
  "namespace " << _namespace_name << " {\n"
  "using namespace pmt::util::parsert;\n"
  "namespace {\n"
  "GenericLexerTables::TableIndexType const STATE_NR_SINK = " << as_hex(_tables._state_nr_sink) << ";\n"
  "GenericLexerTables::TableIndexType const STATE_NR_MIN_DIFF = " << as_hex(_tables._state_nr_min_diff) << ";\n"
  "GenericLexerTables::TableIndexType const PADDING_L = " << as_hex(_tables._padding_l) << ";\n"
  "GenericLexerTables::TableIndexType const STATE_COUNT = " << as_hex(_tables._state_transition_entries.size()) << ";\n"
  "GenericLexerTables::TableIndexType const COMPRESSED_TRANSITION_ENTRY_COUNT = " << as_hex(_tables._compressed_transition_entries.size()) << ";\n"
  "GenericLexerTables::TableIndexType const ACCEPTS_2D_WIDTH = " << as_hex(_tables._accepts_2d_width) << ";\n";
  write_pair_entries(_tables._state_transition_entries, "GenericLexerTables::StateTransitionEntry const STATE_TRANSITION_ENTRIES[STATE_COUNT]");
  write_pair_entries(_tables._compressed_transition_entries, "GenericLexerTables::CompressedTransitionEntry const COMPRESSED_TRANSITION_ENTRIES[COMPRESSED_TRANSITION_ENTRY_COUNT]");
  write_single_entries(_tables._accepts_2d, "GenericLexerTables::TableIndexType const ACCEPTS_2D[STATE_COUNT * ACCEPTS_2D_WIDTH]");
  write_single_entries(_tables._accept_ids, "GenericId::IdType const ACCEPT_IDS[ACCEPTS_2D_WIDTH]");
  write_single_entries(_tables._terminal_names_sv, "std::string_view const TERMINAL_NAMES[ACCEPTS_2D_WIDTH]");
  write_single_entries(_tables._id_names_sv, "std::string_view const ID_NAMES[" + std::to_string(_tables._id_names.size()) + "]");
  _os_source << "}\n"
  "\n";

  write_source_id_to_string_defintion();
  _os_source << "\n";
  write_source_as_generic_lexer_tables_definition();

  _os_source << "}";
  // clang-format on
}

void TableWriter::write_header_id_enum() {
  _os_header << "  enum : pmt::util::parse::GenericId::IdType {\n";
  std::string first_eq = " = 0";
  std::string delim;
  for (std::string const& id_name : _tables._id_names) {
    _os_header << std::exchange(delim, ",\n") << "   " << id_name << std::exchange(first_eq, "");
  }
  _os_header << "\n"
                "  };\n";
}

void TableWriter::write_source_id_to_string_defintion() {
  _os_source << "auto " << _class_name
             << "::id_to_string(GenericId::IdType id_) -> std::string_view {\n"
                " if (GenericId::is_generic_id(id_)) {\n"
                "   return GenericId::id_to_string(id_);\n"
                " }\n"
                " return ID_NAMES[id_];\n"
                "}\n";
}

void TableWriter::write_source_as_generic_lexer_tables_definition() {
  _os_source << "auto " << _class_name
             << "::as_generic_lexer_tables() const -> GenericLexerTables {\n"
                " GenericLexerTables ret;\n"
                " ret._state_nr_sink = STATE_NR_SINK;\n"
                " ret._state_nr_min_diff = STATE_NR_MIN_DIFF;\n"

                " ret._padding_l = PADDING_L;\n"

                " ret._state_count = STATE_COUNT;\n"
                " ret._compressed_transition_entry_count = COMPRESSED_TRANSITION_ENTRY_COUNT;\n"
                " ret._accepts_2d_width = ACCEPTS_2D_WIDTH;\n"

                " ret._state_transition_entries = STATE_TRANSITION_ENTRIES;\n"
                " ret._compressed_transition_entries = COMPRESSED_TRANSITION_ENTRIES;\n"

                " ret._accepts_2d = ACCEPTS_2D;\n"
                " ret._accept_ids = ACCEPT_IDS;\n"

                " ret._terminal_names = &TERMINAL_NAMES[0];\n"
                " ret._id_names = &ID_NAMES[0];\n"

                " return ret;\n"
                "}\n";
}

void TableWriter::write_single_entries(std::vector<std::string_view> const& entries_, std::string const& label_) {
  _os_source << label_ << " = {\n";
  std::string delim;
  for (size_t i = 0; i < entries_.size(); ++i) {
    if (i == 0) {
      _os_source << " ";
    } else {
      _os_source << ",\n ";
    }
    _os_source << '"' << entries_[i] << '"';
  }
  _os_source << "\n};\n";
}

auto TableWriter::as_hex(std::integral auto value_, bool hex_prefix_) -> std::string {
  std::ostringstream os;
  if (hex_prefix_) {
    os << "0x";
  }
  os << std::hex << value_;
  return os.str();
}

};  // namespace pmt::parserbuilder
